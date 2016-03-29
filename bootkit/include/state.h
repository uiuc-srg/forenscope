/* Scanned */
#define bootjacker_ebp 0xdeadbeef
#define bootjacker_esp 0xdeadbeef
#define bootjacker_eip_ret 0xdeadbeef	
#define bootjacker_cr3	0xdeadbeef
#define bootjacker_cr4	0xdeadbeef

/* Scanned from symtab */
#define bootjacker_idt	0xdeadbeef

/* Always fixed */
#define bootjacker_es 	0x7b
#define bootjacker_cs 	0x60
#define bootjacker_ss 	0x68
#define bootjacker_ds 	0x7b
#define bootjacker_fs 	0x00
#define bootjacker_gs 	0x00
#define bootjacker_ldt	0x0000
#define bootjacker_tr	0x0080
#define bootjacker_cr0 	0x8005003b
#define bootjacker_gdt	0xdeadbeef

/* Don't care registers */
#define bootjacker_eax 0xdeadbeef
#define bootjacker_ebx 0xdeadbeef
#define bootjacker_ecx 0xdeadbeef
#define bootjacker_edx 0xdeadbeef
#define bootjacker_esi 0xdeadbeef
#define bootjacker_edi 0xdeadbeef
#define bootjacker_efl 0xdeadbeef	
#define bootjacker_cpl 0xdeadbeef
#define bootjacker_cr2 0xdeadbeef

/* Floating pt regs unused */
