/*
 * Defines and Linux-specific constants
 */

typedef void (*PFV)(void);
typedef void (*PFI)(int);
typedef int  (*PFA)(const char* fmt, ...);

void printf(const char* fmt, ...);
void k_switch_to_new_gdt(void);

#define printk(x, arg...) s_printk(x, ##arg)
#define switch_to_new_gdt() k_switch_to_new_gdt()
extern void (*s_switch_to_new_gdt)(void);
extern void printf (const char *format, ...);
extern struct Xgt_desc_struct *k_idt_descr;
void lookup_stubs(void);
void print_init_task(void);
extern struct task_struct* kk_init_task;

void run_payload(void);
void prep_hardware(void);

//#define printk(...) 
#define printf(...) 

unsigned long strtoul(const char *cp,char **endp,unsigned int base);
