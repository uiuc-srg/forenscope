#include "module/linuxaddr.h"
#include "../include/linux/autoconf.h"

#include "cmostime.h"
#include "zlib/zlib.h" //dont need this anymore
#include "quicklz.h"

#define LINUX_PAGE_OFFSET    CONFIG_PAGE_OFFSET
#define LINUX_PHYS_OFFSET    CONFIG_PHYSICAL_START
#define LINUX_TEXT_START     (LINUX_PAGE_OFFSET + 0x00100000)

#define MEMDUMP_FILE "/mem.img"
#define BYTES_AT_A_TIME 500
#define DUMP_AREA (1024*1024*450)
#define DUMP_SIZE (1024*1024*32)

//#define QEMU
#define MEM_START (1024*1024)
#define MEM_LIMIT 0x8000000
#define CMP_START  1
#define CMP_OFFSET (1024*1024*CMP_START)
#define TESTPATTERN  0xdeadbeef
#define TESTPATTERN1 0xcafedabe
#define DUMPMAX (0x100000 * 1024)

//QLZ stuff
#define QLZ_PACKET_SIZE 512
#define QLZ_SECTOR_SIZE 512
#define QLZ_LOCAL_BUFFER_SIZE (2*QLZ_PACKET_SIZE)

/* System stuff */
#ifndef SIZE_T
#define SIZE_T
    typedef unsigned int size_t;
#endif
void *malloc(size_t size);
void free(void* p);
int strcmp(const char *s1, const char *s2);
void printf (const char *format, ...);
int getchar(void);
int sprintf (char *buffer, const char *format, ...);
void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, int c, size_t count);
int memcmp_helper(const void *cs, const void *ct, size_t count, int showerrs);
int read_cmos_time(int field);

#define memcmp(cs, ct, cnt) memcmp_helper(cs, ct, cnt, 0)
#define show_bad_bits(cs, ct, cnt, n) memcmp_helper(cs, ct, cnt, n)

void do_memup(void);
void do_dmesg(void);
void do_qemu(void);
void do_qemuidle(void);
void do_memwrite_noflush(int* testptr, unsigned int testcnt, unsigned int length);
void do_memwrite(int* testptr, unsigned int testcnt);
void do_memcmp(int* testptr, int testcnt, int length);
void do_memdump(void);
void do_printtime(void);
void do_pcilist(void);
void do_pcifill(void);
void do_fatdump(void);
void do_memcompressdump(void);
int get_next_sector(void);
unsigned int get_time(void);
void syscall_table_checker(void);
void do_vfs(void);

/* Global data */
#define PCI_LIST_SIZE 200
extern char pci_list_str[PCI_LIST_SIZE];
