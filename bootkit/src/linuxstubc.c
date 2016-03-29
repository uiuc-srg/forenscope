#include "linuxstub.h"
#include "module/linuxaddr.h"

/* Conventions:
 * kk_* - Hex address of symbol
 * k_*  - Convenience accessor to symbol
 * s_*  - Stub to symbol
 */
void (*s_switch_to_new_gdt)(void);
int per_cpu__current_task;
int per_cpu__gdt_page;
struct Xgt_desc_struct *k_idt_descr;
struct task_struct* kk_init_task;

void k_switch_to_new_gdt(void)
{
	s_switch_to_new_gdt();
}

