/* Declarations for use by board files for creating devices.  */

#ifndef HW_BOARDS_H
#define HW_BOARDS_H

typedef void QEMUMachineInitFunc(int ram_size, int vga_ram_size,
                                 const char *boot_device, DisplayState *ds,
                                 const char *kernel_filename,
                                 const char *kernel_cmdline,
                                 const char *initrd_filename,
                                 const char *cpu_model);

typedef struct QEMUMachine {
    const char *name;
    const char *desc;
    QEMUMachineInitFunc *init;
    struct QEMUMachine *next;
} QEMUMachine;

int qemu_register_machine(QEMUMachine *m);

/* Axis ETRAX.  */
extern QEMUMachine bareetraxfs_machine;

/* pc.c */
extern QEMUMachine pc_machine;
extern QEMUMachine isapc_machine;

/* ppc.c */
extern QEMUMachine prep_machine;
extern QEMUMachine core99_machine;
extern QEMUMachine heathrow_machine;
extern QEMUMachine ref405ep_machine;
extern QEMUMachine taihu_machine;

/* mips_r4k.c */
extern QEMUMachine mips_machine;

/* mips_malta.c */
extern QEMUMachine mips_malta_machine;

/* mips_pica61.c */
extern QEMUMachine mips_pica61_machine;

/* mips_mipssim.c */
extern QEMUMachine mips_mipssim_machine;

/* shix.c */
extern QEMUMachine shix_machine;

/* r2d.c */
extern QEMUMachine r2d_machine;

/* sun4m.c */
extern QEMUMachine ss5_machine, ss10_machine, ss600mp_machine, ss20_machine;
extern QEMUMachine ss2_machine;
extern QEMUMachine ss1000_machine, ss2000_machine;

/* sun4u.c */
extern QEMUMachine sun4u_machine;

/* integratorcp.c */
extern QEMUMachine integratorcp_machine;

/* versatilepb.c */
extern QEMUMachine versatilepb_machine;
extern QEMUMachine versatileab_machine;

/* realview.c */
extern QEMUMachine realview_machine;

/* spitz.c */
extern QEMUMachine akitapda_machine;
extern QEMUMachine spitzpda_machine;
extern QEMUMachine borzoipda_machine;
extern QEMUMachine terrierpda_machine;

/* palm.c */
extern QEMUMachine palmte_machine;

/* gumstix.c */
extern QEMUMachine connex_machine;
extern QEMUMachine verdex_machine;

/* stellaris.c */
extern QEMUMachine lm3s811evb_machine;
extern QEMUMachine lm3s6965evb_machine;

/* an5206.c */
extern QEMUMachine an5206_machine;

/* mcf5208.c */
extern QEMUMachine mcf5208evb_machine;

/* dummy_m68k.c */
extern QEMUMachine dummy_m68k_machine;

/* mainstone.c */
extern QEMUMachine mainstone2_machine;

#endif
