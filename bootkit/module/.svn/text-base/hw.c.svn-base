#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/cache.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/percpu.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/dmaengine.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/ide.h>
#include <linux/ethtool.h>
#include <linux/serio.h>
#include <linux/platform_device.h>
#include <linux/libps2.h>
#include <asm/segment.h>
#include <asm/suspend.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/processor.h>
#include <asm/rtc.h>
#include "linuxaddr.h"
#include "state.h"
#include "linuxstub.h"
#include "../drivers/input/mouse/psmouse.h"

void early_vga_write(char*);

/*
 * Register numbers.
 */
#define PIC_MASTER_CMD          0x20
#define PIC_MASTER_IMR          0x21
#define MASTER_ICW4_DEFAULT	0x01
#define PIC_SLAVE_CMD		0xa0
#define PIC_SLAVE_IMR		0xa1
#define SLAVE_ICW4_DEFAULT	0x01
#define PIC_CASCADE_IR		2

static inline void io_delay(void)
{
        const u16 DELAY_PORT = 0x80;
        asm volatile("outb %%al,%0" : : "dN" (DELAY_PORT));
}

/*
 * Disable all interrupts at the legacy PIC.
 */
void mask_all_interrupts(void)
{
        outb(0xff, 0xa1);       /* Mask all interrupts on the secondary PIC */
        io_delay();
        outb(0xfb, 0x21);       /* Mask all but cascade on the primary PIC */
        io_delay();
}

/*
 * Reset IGNNE# if asserted in the FPU.
 */
static void reset_coprocessor(void)
{
        outb(0, 0xf0);
        io_delay();
        outb(0, 0xf1);
        io_delay();
}

void udelay(int i)
{
	int del = i * 10000;
	while(del--);
}

void fix_pic(void)
{
/*
 * Linux:
	pic0: irr=02 imr=fa isr=00 hprio=0 irq_base=20 rr_sel=0 elcr=00 fnm=0
	pic1: irr=00 imr=ee isr=00 hprio=0 irq_base=28 rr_sel=0 elcr=0a fnm=0

	irr= interrupt ready register (00=none), imr=mask reg (ff=all masked)
	isr= masked status, hprio= priority, irq_base = base interrupt of controller (important)
	rr_sel = read reg selection (isr if set, irr if 0), elcr =edge/trigger config, 
	fnm=fully nested mode
*/

#define outb_p(x,y) outb(x,y)
	/*
	 * outb_p - this has to work on a wide range of PC hardware.
	 */
	outb_p(0x11, PIC_MASTER_CMD);   /* ICW1: select 8259A-1 init */
	outb_p(0x20 + 0, PIC_MASTER_IMR);       /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */

	outb_p(1U << PIC_CASCADE_IR, PIC_MASTER_IMR);   /* 8259A-1 (the master) has a slave on IR2 */
	outb_p(MASTER_ICW4_DEFAULT, PIC_MASTER_IMR);

	outb(0x0B,PIC_MASTER_CMD);      /* IRR register */
	inb(PIC_MASTER_CMD);
	outb(0x0A,PIC_MASTER_CMD);      /* IRR register */
	outb(0xfa, PIC_MASTER_IMR);

	outb_p(0x11, PIC_SLAVE_CMD);    /* ICW1: select 8259A-2 init */
	outb_p(0x20 + 8, PIC_SLAVE_IMR);        /* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f */
	outb_p(PIC_CASCADE_IR, PIC_SLAVE_IMR);  /* 8259A-2 is a slave on master's IR2 */
	outb_p(SLAVE_ICW4_DEFAULT, PIC_SLAVE_IMR); /* (slave's support for AEOI in flat mode is to be investigated) */
	outb(0xee, PIC_SLAVE_IMR);
}

void fix_kbd(void)
{
	//write_cmd=00 status=1d mode=43 pending=01 
	// || kbd=09c4b808 irq=163742116 base=00000000 it_shift=00000000
	// This is fairly typical, in sysrq, pending is 01 instead of 00
	
	//set mode back to 43
}

void fix_pit(void)
{
	/*
 	count=00002e9c, latched_count=25f1, count_latched=00, status_latched=00, status=00, read_state=03, write_state=03, write_latch=9c, rw_mode=03, mode=02, bcd=00, gate=01, count_load_time=61824fff

count=00010000, latched_count=0000, count_latched=00, status_latched=00, status=00, read_state=00, write_state=00, write_latch=00, rw_mode=00, mode=03, bcd=00, gate=01, count_load_time=53bb354c

count=00008bd3, latched_count=0000, count_latched=00, status_latched=00, status=00, read_state=03, write_state=03, write_latch=d3, rw_mode=03, mode=00, bcd=00, gate=01, count_load_time=58b7ac79

	PIT 0 is the active one... count, latched_count, count_latched don't matter...
	all other values should match with the exception of count_load_time.
	*/
}

void fix_net(void)
{
	struct nic;
	struct nic *s_nic;
	struct net_device* (*s_dev_get_by_name)(char*) = (void*) kk_dev_get_by_name;

	struct net_device* ndev = s_dev_get_by_name("eth0");
	if(ndev)
	{
		early_vga_write("Found netdev ");
		early_vga_write(ndev->name);
		early_vga_write("\n");

		s_nic = (struct nic*) ndev->priv;

		if(ndev->tx_timeout && netif_running(ndev))
			ndev->tx_timeout(ndev);
	} else {
		early_vga_write("Netdev not found\n");
	}
}

struct ide_hwgroup_t;
struct ide_drive_t;

void fix_disk(void)
{
	int i;
	ide_hwgroup_t *disk;
	ide_hwif_t *h;
	ide_hwif_t *s_ide_hwifs = (void*) kk_ide_hwifs;
	int (*s_drive_is_ready)(struct ide_drive_t*) = (void*) kk_drive_is_ready;
	//ide_startstop_t (*s_ide_do_reset)(struct ide_drive_t*) = (void*) kk_ide_do_reset;
	ide_startstop_t (*s_reset_pollfunc)(struct ide_drive_t*) = (void*) kk_reset_pollfunc;

	early_vga_write("Fix disk\n");

	for(i = 0; i < 4; i++)
	{
		h = &(s_ide_hwifs[i]);
		if(h)
		{
			disk = h->hwgroup;
			if(disk && disk->drive)
			{
				early_vga_write("Found disk ");
				early_vga_write(disk->drive->name);	
				early_vga_write("\n");	
				//s_ide_timer_expiry(disk);
				if(disk->busy)
					early_vga_write("Disk is busy\n");
				else
					early_vga_write("Disk is idle\n");

				if(disk->polling)
					early_vga_write("Disk is polling\n");
				else
					early_vga_write("Disk is not polling\n");

				if(s_drive_is_ready((struct ide_drive_t*)disk->drive))
					early_vga_write("Drive is ready\n");
				else
					early_vga_write("Drive is not ready\n");

				//stat = h->INB(IDE_STATUS_REG);

				//disk->busy = 0;
				s_reset_pollfunc((struct ide_drive_t*)disk->drive);
				//s_ide_do_reset((struct ide_drive_t*)disk->drive);
				//s_ide_do_request(disk, IDE_NO_IRQ);
			}
		}
	}
}

struct i8042_port {
        struct serio *serio;
        int irq;
        unsigned char exists;
        signed char mux;
};

char mouseinit[] = { 
0xff, /* Reset */
0xf2, 0xf2, 0xf6, 0xf3, 0xa, 0xe8, 0x0, 0xf3, 
0x14, 0xf3, 0x3c, 0xf3, 0x28, 0xf3, 0x14, 0xf3, 0x14, 0xf3, 0x3c, 
0xf3, 0x28, 0xf3, 0x14, 0xf3, 0x14, 0xf2, 0xe8, 0x0, 0xe8, 0x0, 0xe8, 
0x0, 0xe8, 0x0, 0xe9, 0xf6, 0xe8, 0x0, 0xe6, 0xe6, 0xe6, 0xe9, 0xe8, 
0x0, 0xe7, 0xe7, 0xe7, 0xe9, 0xe8, 0x3, 0xe6, 0xe6, 0xe6, 0xe9, 0xe8, 
0x0, 0xe6, 0xe6, 0xe6, 0xe9, 0xe1, 0xff, 0xf3, 0xc8, 0xf3, 0x64, 0xf3, 
0x50, 0xf2, 0xf3, 0xc8, 0xf3, 0xc8, 0xf3, 0x50, 0xf2, 0xf3, 0xc8, 0xf3, 
0x50, 0xf3, 0x28, 0xf3, 0x64, 0xe8, 0x3, 0xe6, 0xf4, };

void fix_mouse(void)
{
	int i;
	struct platform_device* s_i8042_platform_device = (void*) kk_i8042_platform_device;
	int (*s_i8042_resume)(struct platform_device*) = (void*)kk_i8042_resume;
	int (*s_i8042_controller_reset)(void) = (void*) kk_i8042_controller_reset;
	//int (*s_psmouse_reset)(struct psmouse*) = (void*) kk_psmouse_reset;
	//struct serio_driver *s_psmouse_drv = (void*) kk_psmouse_drv;
	struct i8042_port *s_i8042_ports = (void*) kk_i8042_ports;
#define I8042_AUX_PORT_NO 1
	struct i8042_port *port = (void*) &(s_i8042_ports[I8042_AUX_PORT_NO]);

	struct serio *serio;

	//struct psmouse *psmouse = serio_get_drvdata(serio);

	if((((int) port) & 0xf0000000) != 0xc0000000)
	{
		early_vga_write("Bad serio port\n");
		while(1);
	}

	if(port)
		serio = port->serio;
	else
	{
		early_vga_write("No mouse port\n");
		return;
	}
	
	s_i8042_platform_device->dev.power.power_state.event = PM_EVENT_SUSPEND;

	s_i8042_controller_reset();
	if(s_i8042_resume(s_i8042_platform_device))
		early_vga_write("Cannot resume mouse\n");
	else
		early_vga_write("Fixed mouse\n");

	for(i = 0; i < sizeof(mouseinit); i++)
	{
		if(serio_write(serio, mouseinit[i])) {
			early_vga_write("Cannot reset mouse\n");
		} else {
			//early_vga_write("Reset mouse\n");
		}
	}
}


static inline unsigned int s_get_rtc_time(struct rtc_time *time)
{
        unsigned char ctrl;

        time->tm_sec = CMOS_READ(RTC_SECONDS);
        time->tm_min = CMOS_READ(RTC_MINUTES);
        time->tm_hour = CMOS_READ(RTC_HOURS);
        time->tm_mday = CMOS_READ(RTC_DAY_OF_MONTH);
        time->tm_mon = CMOS_READ(RTC_MONTH);
        time->tm_year = CMOS_READ(RTC_YEAR);
        ctrl = CMOS_READ(RTC_CONTROL);

        if (!(ctrl & RTC_DM_BINARY) || RTC_ALWAYS_BCD)
        {
                BCD_TO_BIN(time->tm_sec);
                BCD_TO_BIN(time->tm_min);
                BCD_TO_BIN(time->tm_hour);
                BCD_TO_BIN(time->tm_mday);
                BCD_TO_BIN(time->tm_mon);
                BCD_TO_BIN(time->tm_year);
        }

        /*
         * Account for differences between how the RTC uses the values
         * and how they are defined in a struct rtc_time;
         */
        if (time->tm_year <= 69)
                time->tm_year += 100;

        time->tm_mon--;

        return RTC_24H;
}

unsigned char rtc_cmos_read(unsigned char addr)
{
        unsigned char val;
        outb_p(addr, RTC_PORT(0));
        val = inb_p(RTC_PORT(1));
        return val;
}

void fix_time(void)
{
	int (*s_do_settimeofday)(struct timespec*) = (void*) kk_do_settimeofday;
	int (*s_rtc_tm_to_time)(struct rtc_time*, time_t*) = (void*) kk_rtc_tm_to_time;
	struct rtc_time tm;
	time_t newtime;
	struct timespec time;
	s_get_rtc_time(&tm);
	s_rtc_tm_to_time(&tm, &newtime);

	time.tv_sec = newtime;
	time.tv_nsec = 0;
	s_do_settimeofday(&time);
}

void fix_msr(void);
void fix_msr()
{
	long long tscval;
	struct timespec uptime;
	int (*s_ktime_get_ts)(struct timespec*) = (void*) kk_ktime_get_ts;
	s_ktime_get_ts(&uptime);

#define loops_per_jiffy kk_loops_per_jiffy
	/* 100 jiffies/sec */
	tscval = uptime.tv_sec * loops_per_jiffy * 100;
	/* fractional jiffies/sec */
	tscval += (uptime.tv_nsec / 100000) * loops_per_jiffy;
	
	early_vga_write("Fix TSC\n");
	write_tsc(tscval >> 32, (unsigned int) tscval);
}

void print_remove_media_warning(void)
{
#define BJ_DELAY 5
	long long delayloop = BJ_DELAY * kk_loops_per_jiffy * 100;
	early_vga_write("Please remove boot media... resuscitating in 5 seconds\n");
	while(delayloop-- > 0);
}

void prep_hardware(void)
{
	early_vga_write("Preparing hardware for restore\n");
	reset_coprocessor();
	fix_pic();
	fix_kbd();
	fix_pit();
	fix_net();
	fix_disk();
	//fix_mouse();
	//fix_time();
	//fix_msr();
	//print_remove_media_warning();
}
