#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>

MODULE_LICENSE("Dual BSD/GPL"); 

static int __init bootkit_init(void)
{	
	printk("Northbridge analyzer\n");
	return 0;
}


static void __exit bootkit_exit(void)
{
}

module_init(bootkit_init);
module_exit(bootkit_exit);
