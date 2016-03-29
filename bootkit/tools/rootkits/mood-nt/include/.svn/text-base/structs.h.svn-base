#ifndef __STRUCTS_H__
#define __STRUCTS_H__
#define EOVERFLOW       75
#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */
#define PACKET_ADD_MEMBERSHIP           1
#define PACKET_MR_PROMISC       1
#define SOL_PACKET      263

typedef long long		loff_t;

typedef unsigned long		ulong;
typedef unsigned char		uchar;
typedef unsigned long long	u64;
typedef long long		s64;
typedef unsigned short  	sa_family_t;

typedef long int		off_t;

struct timespec {
        long  tv_sec;         // seconds 
        long    tv_nsec;        // nanoseconds 
};

struct __old_kernel_stat {
        unsigned short st_dev;
        unsigned short st_ino;
        unsigned short st_mode;
        unsigned short st_nlink;
        unsigned short st_uid;
        unsigned short st_gid;
        unsigned short st_rdev;
        unsigned long  st_size;
        unsigned long  st_atime;
        unsigned long  st_mtime;
        unsigned long  st_ctime;
};

struct stat {
        unsigned long  st_dev;
        unsigned long  st_ino;
        unsigned short st_mode;
        unsigned short st_nlink;
        unsigned short st_uid;
        unsigned short st_gid;
        unsigned long  st_rdev;
        unsigned long  st_size;
        unsigned long  st_blksize;
        unsigned long  st_blocks;
        unsigned long  st_atime;
        unsigned long  st_atime_nsec;
        unsigned long  st_mtime;
        unsigned long  st_mtime_nsec;
        unsigned long  st_ctime;
        unsigned long  st_ctime_nsec;
        unsigned long  __unused4;
        unsigned long  __unused5;
};

struct stat64 {
        unsigned long long      st_dev;
        unsigned char   __pad0[4];

#define STAT64_HAS_BROKEN_ST_INO        1
        unsigned long   __st_ino;

        unsigned int    st_mode;
        unsigned int    st_nlink;

        unsigned long   st_uid;
        unsigned long   st_gid;

        unsigned long long      st_rdev;
        unsigned char   __pad3[4];

        long long       st_size;
        unsigned long   st_blksize;

        unsigned long   st_blocks;      // Number 512-byte blocks allocated. 
        unsigned long   __pad4;         // future possible st_blocks high bits 

        unsigned long   st_atime;
        unsigned long   st_atime_nsec;

        unsigned long   st_mtime;
        unsigned int    st_mtime_nsec;

        unsigned long   st_ctime;
        unsigned long   st_ctime_nsec;

        unsigned long long      st_ino;
};

struct linux_dirent {
        unsigned long   d_ino;
        unsigned long   d_off;
        unsigned short  d_reclen;
        char            d_name[1];
};

struct linux_dirent64 {
        u64             d_ino;
        s64             d_off;
        unsigned short  d_reclen;
        unsigned char   d_type;
        char            d_name[0];
};

struct pt_regs {
        ulong ebx;
        ulong ecx;
        ulong edx;
        ulong esi;
        ulong edi;
        ulong ebp;
        ulong eax;
        ulong xds;
        ulong xes;
        ulong orig_eax;
        ulong eip;
        ulong xcs;
        ulong eflags;
        ulong esp;
        ulong xss;
} __attribute__ ((packed));

/* here defined semi-structs for 2.4 and 2.6 kernels */

/* if 2.6 */

struct thread_info {

	void *task;
	void *exec_domain;
     	unsigned long           flags;          /* low level flags */
        unsigned long           status;         /* thread-synchronous flags */
        unsigned long           cpu;            /* current CPU */
        long	                preempt_count; /* 0 => preemptable, <0 => BUG */
        unsigned long           addr_limit;     /* thread address space:
                                                   0-0xBFFFFFFF for user-thead
                                                   0-0xFFFFFFFF for kernel-thread
                                                */
};	

struct task_26 {
	long state;
	struct thread_info *info;
};

/* else */
struct task_struct {
       volatile long state;    /* -1 unrunnable, 0 runnable, >0 stopped */
       unsigned long flags;    /* per process flags, defined below */
       int sigpending;
       unsigned long addr_limit;        /* thread address space:
                                                 0-0xBFFFFFFF for user-thead
                                                 0-0xFFFFFFFF for kernel-thread
                                        */
};

union task
{
	struct thread_info ti;
	struct task_struct ts;
};


struct sockaddr {
	  unsigned short sa_family;
	  char sa_data[14];
};

struct ifmap {
  unsigned long mem_start;
  unsigned long mem_end;
  unsigned short base_addr;
  unsigned char irq;
  unsigned char dma;
  unsigned char port;
  /* 3 bytes spare */
};


struct ifreq {
#define IFHWADDRLEN     6
#define IF_NAMESIZE     16
#define IFNAMSIZ        IF_NAMESIZE
  union
  {
    char        ifrn_name[IF_NAMESIZE];         /* if name, e.g. "en0" */
  } ifr_ifrn;
  union {
    struct sockaddr ifru_addr;
    struct sockaddr ifru_dstaddr;
    struct sockaddr ifru_broadaddr;
    struct sockaddr ifru_netmask;
    struct  sockaddr ifru_hwaddr;
    short ifru_flags;
    int ifru_ivalue;
    int ifru_mtu;
    struct ifmap ifru_map;
    char ifru_slave[IF_NAMESIZE];       /* Just fits the size */
    char ifru_newname[IF_NAMESIZE];
    char* ifru_data;
  } ifr_ifru;
};

struct packet_mreq {
    int mr_ifindex;
    unsigned short int mr_type;
    unsigned short int mr_alen;
    unsigned char mr_address[8];
};


struct redir
{
        unsigned long inode;
        char *from;
        char *to;
};
struct iface
{
	unsigned char name[IF_NAMESIZE];
	unsigned char user;
	unsigned char real;
};

typedef struct __user_cap_header_struct {
        unsigned long version;
        int pid;
} cap_header;

typedef struct __user_cap_data_struct {
       unsigned long effective,permitted,inheritable;
} cap_data;

struct nlmsghdr
{
        unsigned long           nlmsg_len;      /* Length of message including header */
        unsigned short          nlmsg_type;     /* Message content */
        unsigned short          nlmsg_flags;    /* Additional flags */
        unsigned long           nlmsg_seq;      /* Sequence number */
        unsigned long           nlmsg_pid;      /* Sending process PID */
};


struct iovec {
  void* iov_base;       /* BSD uses caddr_t (1003.1g requires void *) */
  unsigned long iov_len;       /* Must be size_t (1003.1g) */
};

struct msghdr {
  void* msg_name;               /* Socket name */
  unsigned int msg_namelen;                /* Length of name */
  struct iovec* msg_iov;        /* Data blocks */
  unsigned long msg_iovlen;            /* Number of blocks */
  void* msg_control;            /* Per protocol magic (eg BSD file descriptor passing) */
  unsigned long msg_controllen;        /* Length of cmsg list */
  unsigned int msg_flags;
};
struct tcpdiag_sockid
{
        unsigned short   tcpdiag_sport;
        unsigned short   tcpdiag_dport;
        unsigned long   tcpdiag_src[4];
        unsigned long   tcpdiag_dst[4];
        unsigned long   tcpdiag_if;
        unsigned long   tcpdiag_cookie[2];
};
struct tcpdiagmsg
{
        unsigned char    tcpdiag_family;
        unsigned char    tcpdiag_state;
        unsigned char    tcpdiag_timer;
        unsigned char    tcpdiag_retrans;

        struct tcpdiag_sockid id;

        unsigned long   tcpdiag_expires;
        unsigned long   tcpdiag_rqueue;
        unsigned long   tcpdiag_wqueue;
        unsigned long   tcpdiag_uid;
        unsigned long   tcpdiag_inode;
};

struct sockaddr_nl
{
        sa_family_t     nl_family;      /* AF_NETLINK   */
        unsigned short  nl_pad;         /* zero         */
        unsigned int    nl_pid;         /* process pid  */
        unsigned int    nl_groups;      /* multicast groups mask */
};

struct dr_action
{
	unsigned int reg;
	unsigned long addr;
};

struct notifier_block
{
       int (*notifier_call)(struct notifier_block *self, unsigned long, void *);
       struct notifier_block *next;
       int priority;
};
struct die_args {
        struct pt_regs *regs;
        const char *str;
        long err;
        int trapnr;
        int signr;
};
enum die_val {
        DIE_OOPS = 1,
        DIE_INT3,
        DIE_DEBUG,
        DIE_PANIC,
        DIE_NMI,
        DIE_DIE,
        DIE_NMIWATCHDOG,
        DIE_KERNELDEBUG,
        DIE_TRAP,
        DIE_GPF,
        DIE_CALL,
        DIE_NMI_IPI,
        DIE_PAGE_FAULT,
};

/* fine include */
#endif
