#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <asm/segment.h>
#include <asm/suspend.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/processor.h>
#include "linuxaddr.h"
#include "state.h"
#include "linuxstub.h"

int get_init_stack(void)
{
	struct task_struct** cctask = (struct task_struct**) per_cpu__current_task;
	struct task_struct* ctask;
	ctask = *cctask;

	printf("Init stack is %x\n", ctask->stack);

	return ctask->thread.esp;
}	

char *strpbrk(const char *cs, const char *ct)
{
        const char *sc1, *sc2;

        for (sc1 = cs; *sc1 != '\0'; ++sc1) {
                for (sc2 = ct; *sc2 != '\0'; ++sc2) {
                        if (*sc1 == *sc2)
                                return (char *)sc1;
                }
        }
        return NULL;
}
