/* -*- linux-c -*-
 *
 *	$Id: sysrq.c,v 1.15 1998/08/23 14:56:41 mj Exp $
 *
 *	Linux Magic System Request Key Hacks
 *
 *	(c) 1997 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *	based on ideas by Pavel Machek <pavel@atrey.karlin.mff.cuni.cz>
 *
 *	(c) 2000 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
 *	overhauled to use key registration
 *	based upon discusions in irc://irc.openprojects.net/#kernelnewbies
 */

#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/mount.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/reboot.h>
#include <linux/sysrq.h>
#include <linux/kbd_kern.h>
#include <linux/quotaops.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/writeback.h>
#include <linux/buffer_head.h>		/* for fsync_bdev() */
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/vt_kern.h>
#include <linux/workqueue.h>
#include <linux/kexec.h>
#include <linux/irq.h>
#include <linux/hrtimer.h>
#include <linux/syscalls.h>
#include <linux/rtc.h>

#include <asm/ptrace.h>
#include <asm/irq_regs.h>

/* Whether we react on sysrq keys or just ignore them */
int __read_mostly __sysrq_enabled = 1;

static int __read_mostly sysrq_always_enabled;

int sysrq_on(void)
{
	return __sysrq_enabled || sysrq_always_enabled;
}

/*
 * A value of 1 means 'all', other nonzero values are an op mask:
 */
static inline int sysrq_on_mask(int mask)
{
	return sysrq_always_enabled || __sysrq_enabled == 1 ||
						(__sysrq_enabled & mask);
}

static int __init sysrq_always_enabled_setup(char *str)
{
	sysrq_always_enabled = 1;
	printk(KERN_INFO "debug: sysrq always enabled.\n");

	return 1;
}

__setup("sysrq_always_enabled", sysrq_always_enabled_setup);


static void sysrq_handle_loglevel(int key, struct tty_struct *tty)
{
	int i;
	i = key - '0';
	console_loglevel = 7;
	printk("Loglevel set to %d\n", i);
	console_loglevel = i;
}
static struct sysrq_key_op sysrq_loglevel_op = {
	.handler	= sysrq_handle_loglevel,
	.help_msg	= "loglevel0-8",
	.action_msg	= "Changing Loglevel",
	.enable_mask	= SYSRQ_ENABLE_LOG,
};

#ifdef CONFIG_VT
static void sysrq_handle_SAK(int key, struct tty_struct *tty)
{
	struct work_struct *SAK_work = &vc_cons[fg_console].SAK_work;
	schedule_work(SAK_work);
}
static struct sysrq_key_op sysrq_SAK_op = {
	.handler	= sysrq_handle_SAK,
	.help_msg	= "saK",
	.action_msg	= "SAK",
	.enable_mask	= SYSRQ_ENABLE_KEYBOARD,
};
#else
#define sysrq_SAK_op (*(struct sysrq_key_op *)0)
#endif

#ifdef CONFIG_VT
static void sysrq_handle_unraw(int key, struct tty_struct *tty)
{
	struct kbd_struct *kbd = &kbd_table[fg_console];

	if (kbd)
		kbd->kbdmode = VC_XLATE;
}
static struct sysrq_key_op sysrq_unraw_op = {
	.handler	= sysrq_handle_unraw,
	.help_msg	= "unRaw",
	.action_msg	= "Keyboard mode set to XLATE",
	.enable_mask	= SYSRQ_ENABLE_KEYBOARD,
};
#else
#define sysrq_unraw_op (*(struct sysrq_key_op *)0)
#endif /* CONFIG_VT */

#ifdef CONFIG_KEXEC
static void sysrq_handle_crashdump(int key, struct tty_struct *tty)
{
	crash_kexec(get_irq_regs());
}
static struct sysrq_key_op sysrq_crashdump_op = {
	.handler	= sysrq_handle_crashdump,
	.help_msg	= "Crashdump",
	.action_msg	= "Trigger a crashdump",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};
#else
#define sysrq_crashdump_op (*(struct sysrq_key_op *)0)
#endif

static void sysrq_handle_reboot(int key, struct tty_struct *tty)
{
	lockdep_off();
	local_irq_enable();
	emergency_restart();
}
static struct sysrq_key_op sysrq_reboot_op = {
	.handler	= sysrq_handle_reboot,
	.help_msg	= "reBoot",
	.action_msg	= "Resetting",
	.enable_mask	= SYSRQ_ENABLE_BOOT,
};

static void sysrq_handle_test_reboot(int key, struct tty_struct *tty)
{
	int cr0, cr3, cr4;
	int sp;
	int idt[2] = {0x0, 0x0};
	int idtr;

	//printk("Test reboot current=%s, preempt=%d pid=%d cursp=%x asmsp=%x init pgd=%08x\n", current->comm, current_thread_info()->preempt_count, current->pid, (int)current->stack, (int) &sp, init_task.active_mm->pgd);

	asm("mov %%cr0, %0": "=r"(cr0));
	asm("mov %%cr3, %0": "=r"(cr3));
 	asm("mov %%cr4, %0": "=r"(cr4));
	asm("sidt %0" : "=m" (idt));
	idtr = ((idt[1] & 0xffff) << 16) + (idt[0] >> 16);
	printk("emchandbg: cr0=%08x cr3=%08x cr4=%08x sp=%08x idt=%08x:%08x idtr=%08x\n", 
		cr0, cr3, cr4, (int) &sp, idt[0], idt[1], idtr);

	while(1);

	sysrq_handle_reboot(key, tty);
}
static struct sysrq_key_op sysrq_test_reboot_op = {
	.handler	= sysrq_handle_test_reboot,
	.help_msg	= "(z) reboot",
	.action_msg	= "Test resetting",
	.enable_mask	= SYSRQ_ENABLE_BOOT,
};

static void sysrq_handle_sync(int key, struct tty_struct *tty)
{
	emergency_sync();
}
static struct sysrq_key_op sysrq_sync_op = {
	.handler	= sysrq_handle_sync,
	.help_msg	= "Sync",
	.action_msg	= "Emergency Sync",
	.enable_mask	= SYSRQ_ENABLE_SYNC,
};

static void sysrq_handle_show_timers(int key, struct tty_struct *tty)
{
	sysrq_timer_list_show();
}

static struct sysrq_key_op sysrq_show_timers_op = {
	.handler	= sysrq_handle_show_timers,
	.help_msg	= "show-all-timers(Q)",
	.action_msg	= "Show Pending Timers",
};

static void sysrq_handle_mountro(int key, struct tty_struct *tty)
{
	emergency_remount();
}
static struct sysrq_key_op sysrq_mountro_op = {
	.handler	= sysrq_handle_mountro,
	.help_msg	= "Unmount",
	.action_msg	= "Emergency Remount R/O",
	.enable_mask	= SYSRQ_ENABLE_REMOUNT,
};

#ifdef CONFIG_LOCKDEP
static void sysrq_handle_showlocks(int key, struct tty_struct *tty)
{
	debug_show_all_locks();
}

static struct sysrq_key_op sysrq_showlocks_op = {
	.handler	= sysrq_handle_showlocks,
	.help_msg	= "show-all-locks(D)",
	.action_msg	= "Show Locks Held",
};
#else
#define sysrq_showlocks_op (*(struct sysrq_key_op *)0)
#endif

static void sysrq_handle_showregs(int key, struct tty_struct *tty)
{
	struct pt_regs *regs = get_irq_regs();
	if (regs)
		show_regs(regs);
}
static struct sysrq_key_op sysrq_showregs_op = {
	.handler	= sysrq_handle_showregs,
	.help_msg	= "showPc",
	.action_msg	= "Show Regs",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

static void sysrq_handle_showstate(int key, struct tty_struct *tty)
{
	show_state();
}
static struct sysrq_key_op sysrq_showstate_op = {
	.handler	= sysrq_handle_showstate,
	.help_msg	= "showTasks",
	.action_msg	= "Show State",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

static void sysrq_handle_showstate_blocked(int key, struct tty_struct *tty)
{
	show_state_filter(TASK_UNINTERRUPTIBLE);
}
static struct sysrq_key_op sysrq_showstate_blocked_op = {
	.handler	= sysrq_handle_showstate_blocked,
	.help_msg	= "shoW-blocked-tasks",
	.action_msg	= "Show Blocked State",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};


static void sysrq_handle_showmem(int key, struct tty_struct *tty)
{
	show_mem();
}
static struct sysrq_key_op sysrq_showmem_op = {
	.handler	= sysrq_handle_showmem,
	.help_msg	= "showMem",
	.action_msg	= "Show Memory",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

/*
 * Signal sysrq helper function.  Sends a signal to all user processes.
 */
static void send_sig_all(int sig)
{
	struct task_struct *p;

	for_each_process(p) {
		if (p->mm && !is_init(p))
			/* Not swapper, init nor kernel thread */
			force_sig(sig, p);
	}
}

static void sysrq_handle_term(int key, struct tty_struct *tty)
{
	send_sig_all(SIGTERM);
	console_loglevel = 8;
}
static struct sysrq_key_op sysrq_term_op = {
	.handler	= sysrq_handle_term,
	.help_msg	= "tErm",
	.action_msg	= "Terminate All Tasks",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void moom_callback(struct work_struct *ignored)
{
	out_of_memory(&NODE_DATA(0)->node_zonelists[ZONE_NORMAL],
			GFP_KERNEL, 0);
}

static DECLARE_WORK(moom_work, moom_callback);

static void sysrq_handle_moom(int key, struct tty_struct *tty)
{
	schedule_work(&moom_work);
}
static struct sysrq_key_op sysrq_moom_op = {
	.handler	= sysrq_handle_moom,
	.help_msg	= "Full",
	.action_msg	= "Manual OOM execution",
};

static void sysrq_handle_kill(int key, struct tty_struct *tty)
{
	send_sig_all(SIGKILL);
	console_loglevel = 8;
}
static struct sysrq_key_op sysrq_kill_op = {
	.handler	= sysrq_handle_kill,
	.help_msg	= "kIll",
	.action_msg	= "Kill All Tasks",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void sysrq_handle_unrt(int key, struct tty_struct *tty)
{
	normalize_rt_tasks();
}
static struct sysrq_key_op sysrq_unrt_op = {
	.handler	= sysrq_handle_unrt,
	.help_msg	= "Nice",
	.action_msg	= "Nice All RT Tasks",
	.enable_mask	= SYSRQ_ENABLE_RTNICE,
};

#if 0
static int do_myexec(void *ignored)
{
    static char *argv[] = { "sh", NULL, };
    extern char *envp_init[]; /* from init/main.c */

    sys_close(0); sys_close(1); sys_close(2);
    sys_setsid();
    /*(void)sys_open("/dev/ttyS0",O_RDWR,0);*/
    (void)sys_open("/dev/tty4",O_RDWR,0);
    (void)sys_dup(0);
    (void)sys_dup(0);
    return kernel_execve("/bin/sh", argv, envp_init);
}
#endif

static int do_myexec(void *ignored)
{
    struct device *dev;
    struct rtc_device *rtc = NULL;

    if(rtc_class == NULL) {
        return 1;
    }

    down(&rtc_class->sem);
    list_for_each_entry(dev, &rtc_class->devices, node) {
        dev = get_device(dev);
        if(dev) {
            rtc = to_rtc_device(dev);
            if(rtc) {
                printk("dev: [%s]\n", rtc->name);

                if(rtc_class->resume != NULL) {
                    printk("jiffies: 0x%016LX\n", jiffies);
                    rtc_class->resume(&(rtc->dev));
                    printk("jiffies: 0x%016LX\n", jiffies);
                }
            }
        }
        break;
    }
    up(&rtc_class->sem);
    
#if 0
    _cmos = rtc_class_open("rtc_cmos");
    printk("rtc_cmos: 0x%08X\n", rtc_cmos);

    if(rtc_cmos != NULL) {
        printk("rtc_class: 0x%08X\n", rtc_class);

        if(rtc_class != NULL) {
            printk("resume: 0x%08X\n", rtc_class->resume);

            if(rtc_class->resume != NULL) {
                printk("jiffies: 0x%016LX\n", jiffies);
                rtc_class->resume(&(rtc_cmos->dev));
                printk("jiffies: 0x%016LX\n", jiffies);
            }
        }
    }
#endif

    return 0;
}

static void exec_callback(struct work_struct *ignored)
{
    /* XXX: I don't know what the SIGCHLD flag is for. I stole it
     * from handle_initrd. */
    kernel_thread(do_myexec, NULL, SIGCHLD);
    /* Since we don't wait on the new PID returned by kernel_thread
     * we may be causing zombie processes. */
}

static DECLARE_WORK(exec_work, exec_callback);

static void sysrq_handle_exec(int key, struct tty_struct *tty)
{
	schedule_work(&exec_work);
}
static struct sysrq_key_op sysrq_exec_op = {
	.handler	= sysrq_handle_exec,
	.help_msg	= "Yuck",
	.action_msg	= "Execute a shell",
};



/* Key Operations table and lock */
static DEFINE_SPINLOCK(sysrq_key_table_lock);

static struct sysrq_key_op *sysrq_key_table[36] = {
	&sysrq_loglevel_op,		/* 0 */
	&sysrq_loglevel_op,		/* 1 */
	&sysrq_loglevel_op,		/* 2 */
	&sysrq_loglevel_op,		/* 3 */
	&sysrq_loglevel_op,		/* 4 */
	&sysrq_loglevel_op,		/* 5 */
	&sysrq_loglevel_op,		/* 6 */
	&sysrq_loglevel_op,		/* 7 */
	&sysrq_loglevel_op,		/* 8 */
	&sysrq_loglevel_op,		/* 9 */

	/*
	 * a: Don't use for system provided sysrqs, it is handled specially on
	 * sparc and will never arrive.
	 */
	NULL,				/* a */
	&sysrq_reboot_op,		/* b */
	&sysrq_crashdump_op,		/* c & ibm_emac driver debug */
	&sysrq_showlocks_op,		/* d */
	&sysrq_term_op,			/* e */
	&sysrq_moom_op,			/* f */
	/* g: May be registered by ppc for kgdb */
	NULL,				/* g */
	NULL,				/* h */
	&sysrq_kill_op,			/* i */
	NULL,				/* j */
	&sysrq_SAK_op,			/* k */
	NULL,				/* l */
	&sysrq_showmem_op,		/* m */
	&sysrq_unrt_op,			/* n */
	/* o: This will often be registered as 'Off' at init time */
	NULL,				/* o */
	&sysrq_showregs_op,		/* p */
	&sysrq_show_timers_op,		/* q */
	&sysrq_unraw_op,		/* r */
	&sysrq_sync_op,			/* s */
	&sysrq_showstate_op,		/* t */
	&sysrq_mountro_op,		/* u */
	/* v: May be registered at init time by SMP VOYAGER */
	NULL,				/* v */
	&sysrq_showstate_blocked_op,	/* w */
	/* x: May be registered on ppc/powerpc for xmon */
	NULL,				/* x */
	&sysrq_exec_op,			/* y */
	&sysrq_test_reboot_op		/* z */
};

/* key2index calculation, -1 on invalid index */
static int sysrq_key_table_key2index(int key)
{
	int retval;

	if ((key >= '0') && (key <= '9'))
		retval = key - '0';
	else if ((key >= 'a') && (key <= 'z'))
		retval = key + 10 - 'a';
	else
		retval = -1;
	return retval;
}

/*
 * get and put functions for the table, exposed to modules.
 */
struct sysrq_key_op *__sysrq_get_key_op(int key)
{
        struct sysrq_key_op *op_p = NULL;
        int i;

	i = sysrq_key_table_key2index(key);
	if (i != -1)
	        op_p = sysrq_key_table[i];
        return op_p;
}

static void __sysrq_put_key_op(int key, struct sysrq_key_op *op_p)
{
        int i = sysrq_key_table_key2index(key);

        if (i != -1)
                sysrq_key_table[i] = op_p;
}

/*
 * This is the non-locking version of handle_sysrq.  It must/can only be called
 * by sysrq key handlers, as they are inside of the lock
 */
void __handle_sysrq(int key, struct tty_struct *tty, int check_mask)
{
	struct sysrq_key_op *op_p;
	int orig_log_level;
	int i;
	unsigned long flags;

	spin_lock_irqsave(&sysrq_key_table_lock, flags);
	orig_log_level = console_loglevel;
	console_loglevel = 7;
	printk(KERN_INFO "SysRq : ");

        op_p = __sysrq_get_key_op(key);
        if (op_p) {
		/*
		 * Should we check for enabled operations (/proc/sysrq-trigger
		 * should not) and is the invoked operation enabled?
		 */
		if (!check_mask || sysrq_on_mask(op_p->enable_mask)) {
			printk("%s\n", op_p->action_msg);
			console_loglevel = orig_log_level;
			op_p->handler(key, tty);
		} else {
			printk("This sysrq operation is disabled.\n");
		}
	} else {
		printk("HELP : ");
		/* Only print the help msg once per handler */
		for (i = 0; i < ARRAY_SIZE(sysrq_key_table); i++) {
			if (sysrq_key_table[i]) {
				int j;

				for (j = 0; sysrq_key_table[i] !=
						sysrq_key_table[j]; j++)
					;
				if (j != i)
					continue;
				printk("%s ", sysrq_key_table[i]->help_msg);
			}
		}
		printk("\n");
		console_loglevel = orig_log_level;
	}
	spin_unlock_irqrestore(&sysrq_key_table_lock, flags);
}

/*
 * This function is called by the keyboard handler when SysRq is pressed
 * and any other keycode arrives.
 */
void handle_sysrq(int key, struct tty_struct *tty)
{
	if (sysrq_on())
		__handle_sysrq(key, tty, 1);
}
EXPORT_SYMBOL(handle_sysrq);

static int __sysrq_swap_key_ops(int key, struct sysrq_key_op *insert_op_p,
                                struct sysrq_key_op *remove_op_p)
{

	int retval;
	unsigned long flags;

	spin_lock_irqsave(&sysrq_key_table_lock, flags);
	if (__sysrq_get_key_op(key) == remove_op_p) {
		__sysrq_put_key_op(key, insert_op_p);
		retval = 0;
	} else {
		retval = -1;
	}
	spin_unlock_irqrestore(&sysrq_key_table_lock, flags);
	return retval;
}

int register_sysrq_key(int key, struct sysrq_key_op *op_p)
{
	return __sysrq_swap_key_ops(key, op_p, NULL);
}
EXPORT_SYMBOL(register_sysrq_key);

int unregister_sysrq_key(int key, struct sysrq_key_op *op_p)
{
	return __sysrq_swap_key_ops(key, NULL, op_p);
}
EXPORT_SYMBOL(unregister_sysrq_key);
