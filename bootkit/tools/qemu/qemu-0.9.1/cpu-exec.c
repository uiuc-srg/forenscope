/*
 *  i386 emulator main execution loop
 *
 *  Copyright (c) 2003-2005 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "config.h"
#include "exec.h"
#include "disas.h"

#if !defined(CONFIG_SOFTMMU)
#undef EAX
#undef ECX
#undef EDX
#undef EBX
#undef ESP
#undef EBP
#undef ESI
#undef EDI
#undef EIP
#include <signal.h>
#include <sys/ucontext.h>
#endif

int tb_invalidated_flag;

//#define DEBUG_EXEC
//#define DEBUG_SIGNAL

#define SAVE_GLOBALS()
#define RESTORE_GLOBALS()

#if defined(__sparc__) && !defined(HOST_SOLARIS)
#include <features.h>
#if defined(__GLIBC__) && ((__GLIBC__ < 2) || \
                           ((__GLIBC__ == 2) && (__GLIBC_MINOR__ <= 90)))
// Work around ugly bugs in glibc that mangle global register contents

static volatile void *saved_env;
static volatile unsigned long saved_t0, saved_i7;
#undef SAVE_GLOBALS
#define SAVE_GLOBALS() do {                                     \
        saved_env = env;                                        \
        saved_t0 = T0;                                          \
        asm volatile ("st %%i7, [%0]" : : "r" (&saved_i7));     \
    } while(0)

#undef RESTORE_GLOBALS
#define RESTORE_GLOBALS() do {                                  \
        env = (void *)saved_env;                                \
        T0 = saved_t0;                                          \
        asm volatile ("ld [%0], %%i7" : : "r" (&saved_i7));     \
    } while(0)

static int sparc_setjmp(jmp_buf buf)
{
    int ret;

    SAVE_GLOBALS();
    ret = setjmp(buf);
    RESTORE_GLOBALS();
    return ret;
}
#undef setjmp
#define setjmp(jmp_buf) sparc_setjmp(jmp_buf)

static void sparc_longjmp(jmp_buf buf, int val)
{
    SAVE_GLOBALS();
    longjmp(buf, val);
}
#define longjmp(jmp_buf, val) sparc_longjmp(jmp_buf, val)
#endif
#endif

void cpu_loop_exit(void)
{
    /* NOTE: the register at this point must be saved by hand because
       longjmp restore them */
    regs_to_env();
    longjmp(env->jmp_env, 1);
}

#if !(defined(TARGET_SPARC) || defined(TARGET_SH4) || defined(TARGET_M68K))
#define reg_T2
#endif

/* exit the current TB from a signal handler. The host registers are
   restored in a state compatible with the CPU emulator
 */
void cpu_resume_from_signal(CPUState *env1, void *puc)
{
#if !defined(CONFIG_SOFTMMU)
    struct ucontext *uc = puc;
#endif

    env = env1;

    /* XXX: restore cpu registers saved in host registers */

#if !defined(CONFIG_SOFTMMU)
    if (puc) {
        /* XXX: use siglongjmp ? */
        sigprocmask(SIG_SETMASK, &uc->uc_sigmask, NULL);
    }
#endif
    longjmp(env->jmp_env, 1);
}


static TranslationBlock *tb_find_slow(target_ulong pc,
                                      target_ulong cs_base,
                                      uint64_t flags)
{
    TranslationBlock *tb, **ptb1;
    int code_gen_size;
    unsigned int h;
    target_ulong phys_pc, phys_page1, phys_page2, virt_page2;
    uint8_t *tc_ptr;

    spin_lock(&tb_lock);

    tb_invalidated_flag = 0;

    regs_to_env(); /* XXX: do it just before cpu_gen_code() */

    /* find translated block using physical mappings */
    phys_pc = get_phys_addr_code(env, pc);
    phys_page1 = phys_pc & TARGET_PAGE_MASK;
    phys_page2 = -1;
    h = tb_phys_hash_func(phys_pc);
    ptb1 = &tb_phys_hash[h];
    for(;;) {
        tb = *ptb1;
        if (!tb)
            goto not_found;
        if (tb->pc == pc &&
            tb->page_addr[0] == phys_page1 &&
            tb->cs_base == cs_base &&
            tb->flags == flags) {
            /* check next page if needed */
            if (tb->page_addr[1] != -1) {
                virt_page2 = (pc & TARGET_PAGE_MASK) +
                    TARGET_PAGE_SIZE;
                phys_page2 = get_phys_addr_code(env, virt_page2);
                if (tb->page_addr[1] == phys_page2)
                    goto found;
            } else {
                goto found;
            }
        }
        ptb1 = &tb->phys_hash_next;
    }
 not_found:
    /* if no translated code available, then translate it now */
    tb = tb_alloc(pc);
    if (!tb) {
        /* flush must be done */
        tb_flush(env);
        /* cannot fail at this point */
        tb = tb_alloc(pc);
        /* don't forget to invalidate previous TB info */
        tb_invalidated_flag = 1;
    }
    tc_ptr = code_gen_ptr;
    tb->tc_ptr = tc_ptr;
    tb->cs_base = cs_base;
    tb->flags = flags;
    SAVE_GLOBALS();
    cpu_gen_code(env, tb, &code_gen_size);
    RESTORE_GLOBALS();
    code_gen_ptr = (void *)(((unsigned long)code_gen_ptr + code_gen_size + CODE_GEN_ALIGN - 1) & ~(CODE_GEN_ALIGN - 1));

    /* check next page if needed */
    virt_page2 = (pc + tb->size - 1) & TARGET_PAGE_MASK;
    phys_page2 = -1;
    if ((pc & TARGET_PAGE_MASK) != virt_page2) {
        phys_page2 = get_phys_addr_code(env, virt_page2);
    }
    tb_link_phys(tb, phys_pc, phys_page2);

 found:
    /* we add the TB in the virtual pc hash table */
    env->tb_jmp_cache[tb_jmp_cache_hash_func(pc)] = tb;
    spin_unlock(&tb_lock);
    return tb;
}

static inline TranslationBlock *tb_find_fast(void)
{
    TranslationBlock *tb;
    target_ulong cs_base, pc;
    uint64_t flags;

    /* we record a subset of the CPU state. It will
       always be the same before a given translated block
       is executed. */
#if defined(TARGET_I386)
    flags = env->hflags;
    flags |= (env->eflags & (IOPL_MASK | TF_MASK | VM_MASK));
    flags |= env->intercept;
    cs_base = env->segs[R_CS].base;
    pc = cs_base + env->eip;
#elif defined(TARGET_ARM)
    flags = env->thumb | (env->vfp.vec_len << 1)
            | (env->vfp.vec_stride << 4);
    if ((env->uncached_cpsr & CPSR_M) != ARM_CPU_MODE_USR)
        flags |= (1 << 6);
    if (env->vfp.xregs[ARM_VFP_FPEXC] & (1 << 30))
        flags |= (1 << 7);
    flags |= (env->condexec_bits << 8);
    cs_base = 0;
    pc = env->regs[15];
#elif defined(TARGET_SPARC)
#ifdef TARGET_SPARC64
    // Combined FPU enable bits . PRIV . DMMU enabled . IMMU enabled
    flags = (((env->pstate & PS_PEF) >> 1) | ((env->fprs & FPRS_FEF) << 2))
        | (env->pstate & PS_PRIV) | ((env->lsu & (DMMU_E | IMMU_E)) >> 2);
#else
    // FPU enable . Supervisor
    flags = (env->psref << 4) | env->psrs;
#endif
    cs_base = env->npc;
    pc = env->pc;
#elif defined(TARGET_PPC)
    flags = env->hflags;
    cs_base = 0;
    pc = env->nip;
#elif defined(TARGET_MIPS)
    flags = env->hflags & (MIPS_HFLAG_TMASK | MIPS_HFLAG_BMASK);
    cs_base = 0;
    pc = env->PC[env->current_tc];
#elif defined(TARGET_M68K)
    flags = (env->fpcr & M68K_FPCR_PREC)  /* Bit  6 */
            | (env->sr & SR_S)            /* Bit  13 */
            | ((env->macsr >> 4) & 0xf);  /* Bits 0-3 */
    cs_base = 0;
    pc = env->pc;
#elif defined(TARGET_SH4)
    flags = env->flags;
    cs_base = 0;
    pc = env->pc;
#elif defined(TARGET_ALPHA)
    flags = env->ps;
    cs_base = 0;
    pc = env->pc;
#elif defined(TARGET_CRIS)
    flags = 0;
    cs_base = 0;
    pc = env->pc;
#else
#error unsupported CPU
#endif
    tb = env->tb_jmp_cache[tb_jmp_cache_hash_func(pc)];
    if (__builtin_expect(!tb || tb->pc != pc || tb->cs_base != cs_base ||
                         tb->flags != flags, 0)) {
        tb = tb_find_slow(pc, cs_base, flags);
        /* Note: we do it here to avoid a gcc bug on Mac OS X when
           doing it in tb_find_slow */
        if (tb_invalidated_flag) {
            /* as some TB could have been invalidated because
               of memory exceptions while generating the code, we
               must recompute the hash index here */
            T0 = 0;
        }
    }
    return tb;
}

extern unsigned inject;
extern unsigned inject_address;
void do_inject(CPUState *env) {
//    printf("debug stop at pc=0x%x Resuming...\n",env->eip);
    // Read physical memory at specified address and see if it changes
    static unsigned lastval = 0;
    static unsigned preveip = 0;

    unsigned memword;
    uint8_t *membytes = (uint8_t *)&memword;
    // Read virtual memory
//    cpu_memory_rw_debug(env, inject_address, membytes, 4, 0);
    // Read physical memory
    cpu_physical_memory_rw(inject_address, membytes, 4, 0);

    if (lastval == 0) {
        lastval = memword;
        return;
    }

    if (lastval != memword) {
        printf("Memory word at address 0x%x changed from 0x%x to 0x%x by instruction at EIP=0x%x\n", inject_address, lastval, memword, preveip);
        lastval = memword;
    }
    preveip = env->eip;
}

#define BREAK_CHAIN T0 = 0

/* main execution loop */

int cpu_exec(CPUState *env1)
{
#define DECLARE_HOST_REGS 1
#include "hostregs_helper.h"
#if defined(TARGET_SPARC)
#if defined(reg_REGWPTR)
    uint32_t *saved_regwptr;
#endif
#endif
    int ret, interrupt_request;
    void (*gen_func)(void);
    TranslationBlock *tb;
    uint8_t *tc_ptr;

    if (cpu_halted(env1) == EXCP_HALTED)
        return EXCP_HALTED;

    cpu_single_env = env1;

    /* first we save global registers */
#define SAVE_HOST_REGS 1
#include "hostregs_helper.h"
    env = env1;
    SAVE_GLOBALS();

    env_to_regs();
#if defined(TARGET_I386)
    /* put eflags in CPU temporary format */
    CC_SRC = env->eflags & (CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
    DF = 1 - (2 * ((env->eflags >> 10) & 1));
    CC_OP = CC_OP_EFLAGS;
    env->eflags &= ~(DF_MASK | CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
#elif defined(TARGET_SPARC)
#if defined(reg_REGWPTR)
    saved_regwptr = REGWPTR;
#endif
#elif defined(TARGET_M68K)
    env->cc_op = CC_OP_FLAGS;
    env->cc_dest = env->sr & 0xf;
    env->cc_x = (env->sr >> 4) & 1;
#elif defined(TARGET_ALPHA)
#elif defined(TARGET_ARM)
#elif defined(TARGET_PPC)
#elif defined(TARGET_MIPS)
#elif defined(TARGET_SH4)
#elif defined(TARGET_CRIS)
    /* XXXXX */
#else
#error unsupported target CPU
#endif
    env->exception_index = -1;

    /* prepare setjmp context for exception handling */
    for(;;) {
        if (setjmp(env->jmp_env) == 0) {
            env->current_tb = NULL;
            /* if an exception is pending, we execute it here */
            if (env->exception_index >= 0) {
                if (env->exception_index >= EXCP_INTERRUPT) {
                    if (inject && env->exception_index == EXCP_DEBUG) {
                        do_inject(env);
                        env->exception_index = -1;
                    } else {
                        /* exit request from the cpu execution loop */
                        ret = env->exception_index;
                        break;
                    }
                } else if (env->user_mode_only) {
                    /* if user mode only, we simulate a fake exception
                       which will be handled outside the cpu execution
                       loop */
#if defined(TARGET_I386)
                    do_interrupt_user(env->exception_index,
                                      env->exception_is_int,
                                      env->error_code,
                                      env->exception_next_eip);
#endif
                    ret = env->exception_index;
                    break;
                } else {
#if defined(TARGET_I386)
                    /* simulate a real cpu exception. On i386, it can
                       trigger new exceptions, but we do not handle
                       double or triple faults yet. */
                    do_interrupt(env->exception_index,
                                 env->exception_is_int,
                                 env->error_code,
                                 env->exception_next_eip, 0);
                    /* successfully delivered */
                    env->old_exception = -1;
#elif defined(TARGET_PPC)
                    do_interrupt(env);
#elif defined(TARGET_MIPS)
                    do_interrupt(env);
#elif defined(TARGET_SPARC)
                    do_interrupt(env->exception_index);
#elif defined(TARGET_ARM)
                    do_interrupt(env);
#elif defined(TARGET_SH4)
		    do_interrupt(env);
#elif defined(TARGET_ALPHA)
                    do_interrupt(env);
#elif defined(TARGET_CRIS)
                    do_interrupt(env);
#elif defined(TARGET_M68K)
                    do_interrupt(0);
#endif
                }
                env->exception_index = -1;
            }
#ifdef USE_KQEMU
            if (kqemu_is_ok(env) && env->interrupt_request == 0) {
                int ret;
                env->eflags = env->eflags | cc_table[CC_OP].compute_all() | (DF & DF_MASK);
                ret = kqemu_cpu_exec(env);
                /* put eflags in CPU temporary format */
                CC_SRC = env->eflags & (CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
                DF = 1 - (2 * ((env->eflags >> 10) & 1));
                CC_OP = CC_OP_EFLAGS;
                env->eflags &= ~(DF_MASK | CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
                if (ret == 1) {
                    /* exception */
                    longjmp(env->jmp_env, 1);
                } else if (ret == 2) {
                    /* softmmu execution needed */
                } else {
                    if (env->interrupt_request != 0) {
                        /* hardware interrupt will be executed just after */
                    } else {
                        /* otherwise, we restart */
                        longjmp(env->jmp_env, 1);
                    }
                }
            }
#endif

            T0 = 0; /* force lookup of first TB */
            for(;;) {
                SAVE_GLOBALS();
                interrupt_request = env->interrupt_request;
                if (__builtin_expect(interrupt_request, 0)
#if defined(TARGET_I386)
			&& env->hflags & HF_GIF_MASK
#endif
				) {
                    if (interrupt_request & CPU_INTERRUPT_DEBUG) {
                        env->interrupt_request &= ~CPU_INTERRUPT_DEBUG;
                        env->exception_index = EXCP_DEBUG;
                        cpu_loop_exit();
                    }
#if defined(TARGET_ARM) || defined(TARGET_SPARC) || defined(TARGET_MIPS) || \
    defined(TARGET_PPC) || defined(TARGET_ALPHA) || defined(TARGET_CRIS)
                    if (interrupt_request & CPU_INTERRUPT_HALT) {
                        env->interrupt_request &= ~CPU_INTERRUPT_HALT;
                        env->halted = 1;
                        env->exception_index = EXCP_HLT;
                        cpu_loop_exit();
                    }
#endif
#if defined(TARGET_I386)
                    if ((interrupt_request & CPU_INTERRUPT_SMI) &&
                        !(env->hflags & HF_SMM_MASK)) {
                        svm_check_intercept(SVM_EXIT_SMI);
                        env->interrupt_request &= ~CPU_INTERRUPT_SMI;
                        do_smm_enter();
                        BREAK_CHAIN;
                    } else if ((interrupt_request & CPU_INTERRUPT_HARD) &&
                        (env->eflags & IF_MASK || env->hflags & HF_HIF_MASK) &&
                        !(env->hflags & HF_INHIBIT_IRQ_MASK)) {
                        int intno;
                        svm_check_intercept(SVM_EXIT_INTR);
                        env->interrupt_request &= ~(CPU_INTERRUPT_HARD | CPU_INTERRUPT_VIRQ);
                        intno = cpu_get_pic_interrupt(env);
                        if (loglevel & CPU_LOG_TB_IN_ASM) {
                            fprintf(logfile, "Servicing hardware INT=0x%02x\n", intno);
                        }
                        do_interrupt(intno, 0, 0, 0, 1);
                        /* ensure that no TB jump will be modified as
                           the program flow was changed */
                        BREAK_CHAIN;
#if !defined(CONFIG_USER_ONLY)
                    } else if ((interrupt_request & CPU_INTERRUPT_VIRQ) &&
                        (env->eflags & IF_MASK) && !(env->hflags & HF_INHIBIT_IRQ_MASK)) {
                         int intno;
                         /* FIXME: this should respect TPR */
                         env->interrupt_request &= ~CPU_INTERRUPT_VIRQ;
                         svm_check_intercept(SVM_EXIT_VINTR);
                         intno = ldl_phys(env->vm_vmcb + offsetof(struct vmcb, control.int_vector));
                         if (loglevel & CPU_LOG_TB_IN_ASM)
                             fprintf(logfile, "Servicing virtual hardware INT=0x%02x\n", intno);
	                 do_interrupt(intno, 0, 0, -1, 1);
                         stl_phys(env->vm_vmcb + offsetof(struct vmcb, control.int_ctl),
                                  ldl_phys(env->vm_vmcb + offsetof(struct vmcb, control.int_ctl)) & ~V_IRQ_MASK);
                        BREAK_CHAIN;
#endif
                    }
#elif defined(TARGET_PPC)
#if 0
                    if ((interrupt_request & CPU_INTERRUPT_RESET)) {
                        cpu_ppc_reset(env);
                    }
#endif
                    if (interrupt_request & CPU_INTERRUPT_HARD) {
                        ppc_hw_interrupt(env);
                        if (env->pending_interrupts == 0)
                            env->interrupt_request &= ~CPU_INTERRUPT_HARD;
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_MIPS)
                    if ((interrupt_request & CPU_INTERRUPT_HARD) &&
                        (env->CP0_Status & env->CP0_Cause & CP0Ca_IP_mask) &&
                        (env->CP0_Status & (1 << CP0St_IE)) &&
                        !(env->CP0_Status & (1 << CP0St_EXL)) &&
                        !(env->CP0_Status & (1 << CP0St_ERL)) &&
                        !(env->hflags & MIPS_HFLAG_DM)) {
                        /* Raise it */
                        env->exception_index = EXCP_EXT_INTERRUPT;
                        env->error_code = 0;
                        do_interrupt(env);
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_SPARC)
                    if ((interrupt_request & CPU_INTERRUPT_HARD) &&
			(env->psret != 0)) {
			int pil = env->interrupt_index & 15;
			int type = env->interrupt_index & 0xf0;

			if (((type == TT_EXTINT) &&
			     (pil == 15 || pil > env->psrpil)) ||
			    type != TT_EXTINT) {
			    env->interrupt_request &= ~CPU_INTERRUPT_HARD;
			    do_interrupt(env->interrupt_index);
			    env->interrupt_index = 0;
#if !defined(TARGET_SPARC64) && !defined(CONFIG_USER_ONLY)
                            cpu_check_irqs(env);
#endif
                        BREAK_CHAIN;
			}
		    } else if (interrupt_request & CPU_INTERRUPT_TIMER) {
			//do_interrupt(0, 0, 0, 0, 0);
			env->interrupt_request &= ~CPU_INTERRUPT_TIMER;
		    }
#elif defined(TARGET_ARM)
                    if (interrupt_request & CPU_INTERRUPT_FIQ
                        && !(env->uncached_cpsr & CPSR_F)) {
                        env->exception_index = EXCP_FIQ;
                        do_interrupt(env);
                        BREAK_CHAIN;
                    }
                    /* ARMv7-M interrupt return works by loading a magic value
                       into the PC.  On real hardware the load causes the
                       return to occur.  The qemu implementation performs the
                       jump normally, then does the exception return when the
                       CPU tries to execute code at the magic address.
                       This will cause the magic PC value to be pushed to
                       the stack if an interrupt occured at the wrong time.
                       We avoid this by disabling interrupts when
                       pc contains a magic address.  */
                    if (interrupt_request & CPU_INTERRUPT_HARD
                        && ((IS_M(env) && env->regs[15] < 0xfffffff0)
                            || !(env->uncached_cpsr & CPSR_I))) {
                        env->exception_index = EXCP_IRQ;
                        do_interrupt(env);
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_SH4)
                    if (interrupt_request & CPU_INTERRUPT_HARD) {
                        do_interrupt(env);
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_ALPHA)
                    if (interrupt_request & CPU_INTERRUPT_HARD) {
                        do_interrupt(env);
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_CRIS)
                    if (interrupt_request & CPU_INTERRUPT_HARD) {
                        do_interrupt(env);
			env->interrupt_request &= ~CPU_INTERRUPT_HARD;
                        BREAK_CHAIN;
                    }
#elif defined(TARGET_M68K)
                    if (interrupt_request & CPU_INTERRUPT_HARD
                        && ((env->sr & SR_I) >> SR_I_SHIFT)
                            < env->pending_level) {
                        /* Real hardware gets the interrupt vector via an
                           IACK cycle at this point.  Current emulated
                           hardware doesn't rely on this, so we
                           provide/save the vector when the interrupt is
                           first signalled.  */
                        env->exception_index = env->pending_vector;
                        do_interrupt(1);
                        BREAK_CHAIN;
                    }
#endif
                   /* Don't use the cached interupt_request value,
                      do_interrupt may have updated the EXITTB flag. */
                    if (env->interrupt_request & CPU_INTERRUPT_EXITTB) {
                        env->interrupt_request &= ~CPU_INTERRUPT_EXITTB;
                        /* ensure that no TB jump will be modified as
                           the program flow was changed */
                        BREAK_CHAIN;
                    }
                    if (interrupt_request & CPU_INTERRUPT_EXIT) {
                        env->interrupt_request &= ~CPU_INTERRUPT_EXIT;
                        env->exception_index = EXCP_INTERRUPT;
                        cpu_loop_exit();
                    }
                }
#ifdef DEBUG_EXEC
                if ((loglevel & CPU_LOG_TB_CPU)) {
                    /* restore flags in standard format */
                    regs_to_env();
#if defined(TARGET_I386)
                    env->eflags = env->eflags | cc_table[CC_OP].compute_all() | (DF & DF_MASK);
                    cpu_dump_state(env, logfile, fprintf, X86_DUMP_CCOP);
                    env->eflags &= ~(DF_MASK | CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C);
#elif defined(TARGET_ARM)
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_SPARC)
		    REGWPTR = env->regbase + (env->cwp * 16);
		    env->regwptr = REGWPTR;
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_PPC)
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_M68K)
                    cpu_m68k_flush_flags(env, env->cc_op);
                    env->cc_op = CC_OP_FLAGS;
                    env->sr = (env->sr & 0xffe0)
                              | env->cc_dest | (env->cc_x << 4);
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_MIPS)
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_SH4)
		    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_ALPHA)
                    cpu_dump_state(env, logfile, fprintf, 0);
#elif defined(TARGET_CRIS)
                    cpu_dump_state(env, logfile, fprintf, 0);
#else
#error unsupported target CPU
#endif
                }
#endif
                tb = tb_find_fast();
#ifdef DEBUG_EXEC
                if ((loglevel & CPU_LOG_EXEC)) {
                    fprintf(logfile, "Trace 0x%08lx [" TARGET_FMT_lx "] %s\n",
                            (long)tb->tc_ptr, tb->pc,
                            lookup_symbol(tb->pc));
                }
#endif
                RESTORE_GLOBALS();
                /* see if we can patch the calling TB. When the TB
                   spans two pages, we cannot safely do a direct
                   jump. */
                {
                    if (T0 != 0 &&
#if USE_KQEMU
                        (env->kqemu_enabled != 2) &&
#endif
                        tb->page_addr[1] == -1) {
                    spin_lock(&tb_lock);
                    tb_add_jump((TranslationBlock *)(long)(T0 & ~3), T0 & 3, tb);
                    spin_unlock(&tb_lock);
                }
                }
                tc_ptr = tb->tc_ptr;
                env->current_tb = tb;
                /* execute the generated code */
                gen_func = (void *)tc_ptr;
#if defined(__sparc__)
                __asm__ __volatile__("call	%0\n\t"
                                     "mov	%%o7,%%i0"
                                     : /* no outputs */
                                     : "r" (gen_func)
                                     : "i0", "i1", "i2", "i3", "i4", "i5",
                                       "o0", "o1", "o2", "o3", "o4", "o5",
                                       "l0", "l1", "l2", "l3", "l4", "l5",
                                       "l6", "l7");
#elif defined(__arm__)
                asm volatile ("mov pc, %0\n\t"
                              ".global exec_loop\n\t"
                              "exec_loop:\n\t"
                              : /* no outputs */
                              : "r" (gen_func)
                              : "r1", "r2", "r3", "r8", "r9", "r10", "r12", "r14");
#elif defined(__ia64)
		struct fptr {
			void *ip;
			void *gp;
		} fp;

		fp.ip = tc_ptr;
		fp.gp = code_gen_buffer + 2 * (1 << 20);
		(*(void (*)(void)) &fp)();
#else
                gen_func();
#endif
                env->current_tb = NULL;
                /* reset soft MMU for next block (it can currently
                   only be set by a memory fault) */
#if defined(TARGET_I386) && !defined(CONFIG_SOFTMMU)
                if (env->hflags & HF_SOFTMMU_MASK) {
                    env->hflags &= ~HF_SOFTMMU_MASK;
                    /* do not allow linking to another block */
                    T0 = 0;
                }
#endif
#if defined(USE_KQEMU)
#define MIN_CYCLE_BEFORE_SWITCH (100 * 1000)
                if (kqemu_is_ok(env) &&
                    (cpu_get_time_fast() - env->last_io_time) >= MIN_CYCLE_BEFORE_SWITCH) {
                    cpu_loop_exit();
                }
#endif
            } /* for(;;) */
        } else {
            env_to_regs();
        }
    } /* for(;;) */


#if defined(TARGET_I386)
    /* restore flags in standard format */
    env->eflags = env->eflags | cc_table[CC_OP].compute_all() | (DF & DF_MASK);
#elif defined(TARGET_ARM)
    /* XXX: Save/restore host fpu exception state?.  */
#elif defined(TARGET_SPARC)
#if defined(reg_REGWPTR)
    REGWPTR = saved_regwptr;
#endif
#elif defined(TARGET_PPC)
#elif defined(TARGET_M68K)
    cpu_m68k_flush_flags(env, env->cc_op);
    env->cc_op = CC_OP_FLAGS;
    env->sr = (env->sr & 0xffe0)
              | env->cc_dest | (env->cc_x << 4);
#elif defined(TARGET_MIPS)
#elif defined(TARGET_SH4)
#elif defined(TARGET_ALPHA)
#elif defined(TARGET_CRIS)
    /* XXXXX */
#else
#error unsupported target CPU
#endif

    /* restore global registers */
    RESTORE_GLOBALS();
#include "hostregs_helper.h"

    /* fail safe : never use cpu_single_env outside cpu_exec() */
    cpu_single_env = NULL;
    return ret;
}

/* must only be called from the generated code as an exception can be
   generated */
void tb_invalidate_page_range(target_ulong start, target_ulong end)
{
    /* XXX: cannot enable it yet because it yields to MMU exception
       where NIP != read address on PowerPC */
#if 0
    target_ulong phys_addr;
    phys_addr = get_phys_addr_code(env, start);
    tb_invalidate_phys_page_range(phys_addr, phys_addr + end - start, 0);
#endif
}

#if defined(TARGET_I386) && defined(CONFIG_USER_ONLY)

void cpu_x86_load_seg(CPUX86State *s, int seg_reg, int selector)
{
    CPUX86State *saved_env;

    saved_env = env;
    env = s;
    if (!(env->cr[0] & CR0_PE_MASK) || (env->eflags & VM_MASK)) {
        selector &= 0xffff;
        cpu_x86_load_seg_cache(env, seg_reg, selector,
                               (selector << 4), 0xffff, 0);
    } else {
        load_seg(seg_reg, selector);
    }
    env = saved_env;
}

void cpu_x86_fsave(CPUX86State *s, target_ulong ptr, int data32)
{
    CPUX86State *saved_env;

    saved_env = env;
    env = s;

    helper_fsave(ptr, data32);

    env = saved_env;
}

void cpu_x86_frstor(CPUX86State *s, target_ulong ptr, int data32)
{
    CPUX86State *saved_env;

    saved_env = env;
    env = s;

    helper_frstor(ptr, data32);

    env = saved_env;
}

#endif /* TARGET_I386 */

#if !defined(CONFIG_SOFTMMU)

#if defined(TARGET_I386)

/* 'pc' is the host PC at which the exception was raised. 'address' is
   the effective address of the memory exception. 'is_write' is 1 if a
   write caused the exception and otherwise 0'. 'old_set' is the
   signal set which should be restored */
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    qemu_printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
                pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_x86_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */
    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    if (ret == 1) {
#if 0
        printf("PF exception: EIP=0x%08x CR2=0x%08x error=0x%x\n",
               env->eip, env->cr[2], env->error_code);
#endif
        /* we restore the process signal mask as the sigreturn should
           do it (XXX: use sigsetjmp) */
        sigprocmask(SIG_SETMASK, old_set, NULL);
        raise_exception_err(env->exception_index, env->error_code);
    } else {
        /* activate soft MMU for this block */
        env->hflags |= HF_SOFTMMU_MASK;
        cpu_resume_from_signal(env, puc);
    }
    /* never comes here */
    return 1;
}

#elif defined(TARGET_ARM)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }
    /* see if it is an MMU fault */
    ret = cpu_arm_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */
    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
}
#elif defined(TARGET_SPARC)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }
    /* see if it is an MMU fault */
    ret = cpu_sparc_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */
    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
}
#elif defined (TARGET_PPC)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_ppc_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */

    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    if (ret == 1) {
#if 0
        printf("PF exception: NIP=0x%08x error=0x%x %p\n",
               env->nip, env->error_code, tb);
#endif
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
        sigprocmask(SIG_SETMASK, old_set, NULL);
        do_raise_exception_err(env->exception_index, env->error_code);
    } else {
        /* activate soft MMU for this block */
        cpu_resume_from_signal(env, puc);
    }
    /* never comes here */
    return 1;
}

#elif defined(TARGET_M68K)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(address, pc, puc)) {
        return 1;
    }
    /* see if it is an MMU fault */
    ret = cpu_m68k_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */
    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
    /* never comes here */
    return 1;
}

#elif defined (TARGET_MIPS)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_mips_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */

    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
    if (ret == 1) {
#if 0
        printf("PF exception: PC=0x" TARGET_FMT_lx " error=0x%x %p\n",
               env->PC, env->error_code, tb);
#endif
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
        sigprocmask(SIG_SETMASK, old_set, NULL);
        do_raise_exception_err(env->exception_index, env->error_code);
    } else {
        /* activate soft MMU for this block */
        cpu_resume_from_signal(env, puc);
    }
    /* never comes here */
    return 1;
}

#elif defined (TARGET_SH4)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_sh4_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */

    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
#if 0
        printf("PF exception: NIP=0x%08x error=0x%x %p\n",
               env->nip, env->error_code, tb);
#endif
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
    /* never comes here */
    return 1;
}

#elif defined (TARGET_ALPHA)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_alpha_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */

    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
#if 0
        printf("PF exception: NIP=0x%08x error=0x%x %p\n",
               env->nip, env->error_code, tb);
#endif
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
    /* never comes here */
    return 1;
}
#elif defined (TARGET_CRIS)
static inline int handle_cpu_signal(unsigned long pc, unsigned long address,
                                    int is_write, sigset_t *old_set,
                                    void *puc)
{
    TranslationBlock *tb;
    int ret;

    if (cpu_single_env)
        env = cpu_single_env; /* XXX: find a correct solution for multithread */
#if defined(DEBUG_SIGNAL)
    printf("qemu: SIGSEGV pc=0x%08lx address=%08lx w=%d oldset=0x%08lx\n",
           pc, address, is_write, *(unsigned long *)old_set);
#endif
    /* XXX: locking issue */
    if (is_write && page_unprotect(h2g(address), pc, puc)) {
        return 1;
    }

    /* see if it is an MMU fault */
    ret = cpu_cris_handle_mmu_fault(env, address, is_write, MMU_USER_IDX, 0);
    if (ret < 0)
        return 0; /* not an MMU fault */
    if (ret == 0)
        return 1; /* the MMU fault was handled without causing real CPU fault */

    /* now we have a real cpu fault */
    tb = tb_find_pc(pc);
    if (tb) {
        /* the PC is inside the translated code. It means that we have
           a virtual CPU fault */
        cpu_restore_state(tb, env, pc, puc);
    }
#if 0
        printf("PF exception: NIP=0x%08x error=0x%x %p\n",
               env->nip, env->error_code, tb);
#endif
    /* we restore the process signal mask as the sigreturn should
       do it (XXX: use sigsetjmp) */
    sigprocmask(SIG_SETMASK, old_set, NULL);
    cpu_loop_exit();
    /* never comes here */
    return 1;
}

#else
#error unsupported target CPU
#endif

#if defined(__i386__)

#if defined(__APPLE__)
# include <sys/ucontext.h>

# define EIP_sig(context)  (*((unsigned long*)&(context)->uc_mcontext->ss.eip))
# define TRAP_sig(context)    ((context)->uc_mcontext->es.trapno)
# define ERROR_sig(context)   ((context)->uc_mcontext->es.err)
#else
# define EIP_sig(context)     ((context)->uc_mcontext.gregs[REG_EIP])
# define TRAP_sig(context)    ((context)->uc_mcontext.gregs[REG_TRAPNO])
# define ERROR_sig(context)   ((context)->uc_mcontext.gregs[REG_ERR])
#endif

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;
    int trapno;

#ifndef REG_EIP
/* for glibc 2.1 */
#define REG_EIP    EIP
#define REG_ERR    ERR
#define REG_TRAPNO TRAPNO
#endif
    pc = EIP_sig(uc);
    trapno = TRAP_sig(uc);
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             trapno == 0xe ?
                             (ERROR_sig(uc) >> 1) & 1 : 0,
                             &uc->uc_sigmask, puc);
}

#elif defined(__x86_64__)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;

    pc = uc->uc_mcontext.gregs[REG_RIP];
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             uc->uc_mcontext.gregs[REG_TRAPNO] == 0xe ?
                             (uc->uc_mcontext.gregs[REG_ERR] >> 1) & 1 : 0,
                             &uc->uc_sigmask, puc);
}

#elif defined(__powerpc__)

/***********************************************************************
 * signal context platform-specific definitions
 * From Wine
 */
#ifdef linux
/* All Registers access - only for local access */
# define REG_sig(reg_name, context)		((context)->uc_mcontext.regs->reg_name)
/* Gpr Registers access  */
# define GPR_sig(reg_num, context)		REG_sig(gpr[reg_num], context)
# define IAR_sig(context)			REG_sig(nip, context)	/* Program counter */
# define MSR_sig(context)			REG_sig(msr, context)   /* Machine State Register (Supervisor) */
# define CTR_sig(context)			REG_sig(ctr, context)   /* Count register */
# define XER_sig(context)			REG_sig(xer, context) /* User's integer exception register */
# define LR_sig(context)			REG_sig(link, context) /* Link register */
# define CR_sig(context)			REG_sig(ccr, context) /* Condition register */
/* Float Registers access  */
# define FLOAT_sig(reg_num, context)		(((double*)((char*)((context)->uc_mcontext.regs+48*4)))[reg_num])
# define FPSCR_sig(context)			(*(int*)((char*)((context)->uc_mcontext.regs+(48+32*2)*4)))
/* Exception Registers access */
# define DAR_sig(context)			REG_sig(dar, context)
# define DSISR_sig(context)			REG_sig(dsisr, context)
# define TRAP_sig(context)			REG_sig(trap, context)
#endif /* linux */

#ifdef __APPLE__
# include <sys/ucontext.h>
typedef struct ucontext SIGCONTEXT;
/* All Registers access - only for local access */
# define REG_sig(reg_name, context)		((context)->uc_mcontext->ss.reg_name)
# define FLOATREG_sig(reg_name, context)	((context)->uc_mcontext->fs.reg_name)
# define EXCEPREG_sig(reg_name, context)	((context)->uc_mcontext->es.reg_name)
# define VECREG_sig(reg_name, context)		((context)->uc_mcontext->vs.reg_name)
/* Gpr Registers access */
# define GPR_sig(reg_num, context)		REG_sig(r##reg_num, context)
# define IAR_sig(context)			REG_sig(srr0, context)	/* Program counter */
# define MSR_sig(context)			REG_sig(srr1, context)  /* Machine State Register (Supervisor) */
# define CTR_sig(context)			REG_sig(ctr, context)
# define XER_sig(context)			REG_sig(xer, context) /* Link register */
# define LR_sig(context)			REG_sig(lr, context)  /* User's integer exception register */
# define CR_sig(context)			REG_sig(cr, context)  /* Condition register */
/* Float Registers access */
# define FLOAT_sig(reg_num, context)		FLOATREG_sig(fpregs[reg_num], context)
# define FPSCR_sig(context)			((double)FLOATREG_sig(fpscr, context))
/* Exception Registers access */
# define DAR_sig(context)			EXCEPREG_sig(dar, context)     /* Fault registers for coredump */
# define DSISR_sig(context)			EXCEPREG_sig(dsisr, context)
# define TRAP_sig(context)			EXCEPREG_sig(exception, context) /* number of powerpc exception taken */
#endif /* __APPLE__ */

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;
    int is_write;

    pc = IAR_sig(uc);
    is_write = 0;
#if 0
    /* ppc 4xx case */
    if (DSISR_sig(uc) & 0x00800000)
        is_write = 1;
#else
    if (TRAP_sig(uc) != 0x400 && (DSISR_sig(uc) & 0x02000000))
        is_write = 1;
#endif
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write, &uc->uc_sigmask, puc);
}

#elif defined(__alpha__)

int cpu_signal_handler(int host_signum, void *pinfo,
                           void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    uint32_t *pc = uc->uc_mcontext.sc_pc;
    uint32_t insn = *pc;
    int is_write = 0;

    /* XXX: need kernel patch to get write flag faster */
    switch (insn >> 26) {
    case 0x0d: // stw
    case 0x0e: // stb
    case 0x0f: // stq_u
    case 0x24: // stf
    case 0x25: // stg
    case 0x26: // sts
    case 0x27: // stt
    case 0x2c: // stl
    case 0x2d: // stq
    case 0x2e: // stl_c
    case 0x2f: // stq_c
	is_write = 1;
    }

    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write, &uc->uc_sigmask, puc);
}
#elif defined(__sparc__)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    uint32_t *regs = (uint32_t *)(info + 1);
    void *sigmask = (regs + 20);
    unsigned long pc;
    int is_write;
    uint32_t insn;

    /* XXX: is there a standard glibc define ? */
    pc = regs[1];
    /* XXX: need kernel patch to get write flag faster */
    is_write = 0;
    insn = *(uint32_t *)pc;
    if ((insn >> 30) == 3) {
      switch((insn >> 19) & 0x3f) {
      case 0x05: // stb
      case 0x06: // sth
      case 0x04: // st
      case 0x07: // std
      case 0x24: // stf
      case 0x27: // stdf
      case 0x25: // stfsr
	is_write = 1;
	break;
      }
    }
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write, sigmask, NULL);
}

#elif defined(__arm__)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;
    int is_write;

    pc = uc->uc_mcontext.gregs[R15];
    /* XXX: compute is_write */
    is_write = 0;
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write,
                             &uc->uc_sigmask, puc);
}

#elif defined(__mc68000)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;
    int is_write;

    pc = uc->uc_mcontext.gregs[16];
    /* XXX: compute is_write */
    is_write = 0;
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write,
                             &uc->uc_sigmask, puc);
}

#elif defined(__ia64)

#ifndef __ISR_VALID
  /* This ought to be in <bits/siginfo.h>... */
# define __ISR_VALID	1
#endif

int cpu_signal_handler(int host_signum, void *pinfo, void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long ip;
    int is_write = 0;

    ip = uc->uc_mcontext.sc_ip;
    switch (host_signum) {
      case SIGILL:
      case SIGFPE:
      case SIGSEGV:
      case SIGBUS:
      case SIGTRAP:
	  if (info->si_code && (info->si_segvflags & __ISR_VALID))
	      /* ISR.W (write-access) is bit 33:  */
	      is_write = (info->si_isr >> 33) & 1;
	  break;

      default:
	  break;
    }
    return handle_cpu_signal(ip, (unsigned long)info->si_addr,
                             is_write,
                             &uc->uc_sigmask, puc);
}

#elif defined(__s390__)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    unsigned long pc;
    int is_write;

    pc = uc->uc_mcontext.psw.addr;
    /* XXX: compute is_write */
    is_write = 0;
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write, &uc->uc_sigmask, puc);
}

#elif defined(__mips__)

int cpu_signal_handler(int host_signum, void *pinfo,
                       void *puc)
{
    siginfo_t *info = pinfo;
    struct ucontext *uc = puc;
    greg_t pc = uc->uc_mcontext.pc;
    int is_write;

    /* XXX: compute is_write */
    is_write = 0;
    return handle_cpu_signal(pc, (unsigned long)info->si_addr,
                             is_write, &uc->uc_sigmask, puc);
}

#else

#error host CPU specific signal handler needed

#endif

#endif /* !defined(CONFIG_SOFTMMU) */
