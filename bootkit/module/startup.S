#include <linux/linkage.h>
#include <asm/desc.h>
#include <asm/segment.h>
#include <asm/cache.h>
#include <asm/page.h>
#include "linuxaddr.h"
#include "state.h"

.globl asm_setup
asm_setup:
	/* Called from resuscitate */
	/* Clear direction */
        cld

	/* Set up an initial address space */
        lgdt boot_gdt_descr 
        movl $(__BOOT_DS),%eax
        movl %eax,%ds
        movl %eax,%es
        movl %eax,%fs
        movl %eax,%gs

#if 0
/*
 * Initialize page tables.  This creates a PDE and a set of page
 * tables, which are located immediately beyond _end.  The variable
 * init_pg_tables_end is set up to point to the first "safe" location.
 * Mappings are created both at virtual address 0 (identity mapping)
 * and PAGE_OFFSET for up to _end+sizeof(page tables)+INIT_MAP_BEYOND_END.
 *
 * Warning: don't use %esi or the stack in this code.  However, %esp
 * can be used as a GPR if you really need it...
 */
page_pde_offset = (__PAGE_OFFSET >> 20);

        movl $(kk_pg0 - __PAGE_OFFSET), %edi
        movl $(kk_swapper_pg_dir - __PAGE_OFFSET), %edx
        movl $0x007, %eax                       /* 0x007 = PRESENT+RW+USER */
10:
        leal 0x007(%edi),%ecx                   /* Create PDE entry */
        movl %ecx,(%edx)                        /* Store identity PDE entry */
        movl %ecx,page_pde_offset(%edx)         /* Store kernel PDE entry */
        addl $4,%edx
        movl $1024, %ecx
11:
        stosl
        addl $0x1000,%eax
        loop 11b
        /* End condition: we must map up to and including INIT_MAP_BEYOND_END */
        /* bytes beyond the end of our own page tables; the +0x007 is the attribute bits */
        leal (kk_INIT_MAP_BEYOND_END+0x007)(%edi),%ebp

	/* emchan: Account for high address */
//	addl $0x8000000, %ebp

        cmpl %ebp,%eax
        jb 10b
        movl %edi,(kk_init_pg_tables_end - __PAGE_OFFSET)
#endif
        xorl %ebx,%ebx                          /* This is the boot CPU (BSP) */
        jmp 3f

3:

enable_paging:
/*
 * Enable paging
 */
        //movl $kk_swapper_pg_dir-__PAGE_OFFSET,%eax
        movl $((kk_page_buf & 0xfffff000) + 4096 - __PAGE_OFFSET),%eax
        movl %eax,%cr3          /* set the page table pointer.. */
        movl %cr0,%eax
        orl $0x80000000,%eax
        movl %eax,%cr0          /* ..and set paging (PG) bit */
        ljmp $__BOOT_CS,$1f     /* Clear prefetch and normalize %eip */
1:

//emchan_halt_loop:
//	jmp emchan_halt_loop

	/* Prepare symbol lookup */
	call prep_symtab_lookup

	/* Prepare a restore context */
	call prep_restore_context

	/* Patch restore context */
	call patch_restore_context

	/* Reinit hardware */
	call prep_hardware

	/* Do restore */
	call do_restore

#ifdef DEBUG_STACK
	call post_stack

	call print_stack

	call get_init_stack
	call print_int_val 
	
	movl  %eax, %esp
	call print_stack

	nop
	nop
	nop
	nop
#endif 

halt:
	jmp halt

/*
 * The IDT and GDT 'descriptors' are a strange 48-bit object
 * only used by the lidt and lgdt instructions. They are not
 * like usual segment descriptors - they consist of a 16-bit
 * segment size, and 32-bit linear address value:
 */

.globl boot_gdt_descr

	ALIGN
# early boot GDT descriptor (must use 1:1 address mapping)
	.word 0				# 32 bit align gdt_desc.address
boot_gdt_descr:
	.word __BOOT_DS+7
	.long boot_gdt 
	/* .long boot_gdt - __PAGE_OFFSET */

# boot GDT descriptor (later on used by CPU#0):
	.word 0				# 32 bit align gdt_desc.address
ENTRY(early_gdt_descr)
	.word GDT_ENTRIES*8-1
	.long k_per_cpu__gdt_page		/* Overwritten for secondary CPUs */

/*
 * The boot_gdt must mirror the equivalent in setup.S and is
 * used only for booting.
 */
	.align L1_CACHE_BYTES
ENTRY(boot_gdt)
	.fill GDT_ENTRY_BOOT_CS,8,0
	.quad 0x00cf9a000000ffff	/* kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff	/* kernel 4GB data at 0x00000000 */

k_per_cpu__gdt_page:
