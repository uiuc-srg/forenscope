/*
 *  SH4 translation
 *
 *  Copyright (c) 2005 Samuel Tardieu
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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define DEBUG_DISAS
#define SH4_DEBUG_DISAS
//#define SH4_SINGLE_STEP

#include "cpu.h"
#include "exec-all.h"
#include "disas.h"

enum {
#define DEF(s, n, copy_size) INDEX_op_ ## s,
#include "opc.h"
#undef DEF
    NB_OPS,
};

#ifdef USE_DIRECT_JUMP
#define TBPARAM(x)
#else
#define TBPARAM(x) ((long)(x))
#endif

static uint16_t *gen_opc_ptr;
static uint32_t *gen_opparam_ptr;

#include "gen-op.h"

typedef struct DisasContext {
    struct TranslationBlock *tb;
    target_ulong pc;
    uint32_t sr;
    uint32_t fpscr;
    uint16_t opcode;
    uint32_t flags;
    int bstate;
    int memidx;
    uint32_t delayed_pc;
    int singlestep_enabled;
} DisasContext;

enum {
    BS_NONE     = 0, /* We go out of the TB without reaching a branch or an
                      * exception condition
                      */
    BS_STOP     = 1, /* We want to stop translation for any reason */
    BS_BRANCH   = 2, /* We reached a branch condition     */
    BS_EXCP     = 3, /* We reached an exception condition */
};

#ifdef CONFIG_USER_ONLY

#define GEN_OP_LD(width, reg) \
  void gen_op_ld##width##_T0_##reg (DisasContext *ctx) { \
    gen_op_ld##width##_T0_##reg##_raw(); \
  }
#define GEN_OP_ST(width, reg) \
  void gen_op_st##width##_##reg##_T1 (DisasContext *ctx) { \
    gen_op_st##width##_##reg##_T1_raw(); \
  }

#else

#define GEN_OP_LD(width, reg) \
  void gen_op_ld##width##_T0_##reg (DisasContext *ctx) { \
    if (ctx->memidx) gen_op_ld##width##_T0_##reg##_kernel(); \
    else gen_op_ld##width##_T0_##reg##_user();\
  }
#define GEN_OP_ST(width, reg) \
  void gen_op_st##width##_##reg##_T1 (DisasContext *ctx) { \
    if (ctx->memidx) gen_op_st##width##_##reg##_T1_kernel(); \
    else gen_op_st##width##_##reg##_T1_user();\
  }

#endif

GEN_OP_LD(ub, T0)
GEN_OP_LD(b, T0)
GEN_OP_ST(b, T0)
GEN_OP_LD(uw, T0)
GEN_OP_LD(w, T0)
GEN_OP_ST(w, T0)
GEN_OP_LD(l, T0)
GEN_OP_ST(l, T0)
GEN_OP_LD(fl, FT0)
GEN_OP_ST(fl, FT0)
GEN_OP_LD(fq, DT0)
GEN_OP_ST(fq, DT0)

void cpu_dump_state(CPUState * env, FILE * f,
		    int (*cpu_fprintf) (FILE * f, const char *fmt, ...),
		    int flags)
{
    int i;
    cpu_fprintf(f, "pc=0x%08x sr=0x%08x pr=0x%08x fpscr=0x%08x\n",
		env->pc, env->sr, env->pr, env->fpscr);
    for (i = 0; i < 24; i += 4) {
	cpu_fprintf(f, "r%d=0x%08x r%d=0x%08x r%d=0x%08x r%d=0x%08x\n",
		    i, env->gregs[i], i + 1, env->gregs[i + 1],
		    i + 2, env->gregs[i + 2], i + 3, env->gregs[i + 3]);
    }
    if (env->flags & DELAY_SLOT) {
	cpu_fprintf(f, "in delay slot (delayed_pc=0x%08x)\n",
		    env->delayed_pc);
    } else if (env->flags & DELAY_SLOT_CONDITIONAL) {
	cpu_fprintf(f, "in conditional delay slot (delayed_pc=0x%08x)\n",
		    env->delayed_pc);
    }
}

void cpu_sh4_reset(CPUSH4State * env)
{
#if defined(CONFIG_USER_ONLY)
    env->sr = SR_FD;            /* FD - kernel does lazy fpu context switch */
#else
    env->sr = 0x700000F0;	/* MD, RB, BL, I3-I0 */
#endif
    env->vbr = 0;
    env->pc = 0xA0000000;
#if defined(CONFIG_USER_ONLY)
    env->fpscr = FPSCR_PR; /* value for userspace according to the kernel */
    set_float_rounding_mode(float_round_nearest_even, &env->fp_status); /* ?! */
#else
    env->fpscr = 0x00040001; /* CPU reset value according to SH4 manual */
    set_float_rounding_mode(float_round_to_zero, &env->fp_status);
#endif
    env->mmucr = 0;
}

CPUSH4State *cpu_sh4_init(const char *cpu_model)
{
    CPUSH4State *env;

    env = qemu_mallocz(sizeof(CPUSH4State));
    if (!env)
	return NULL;
    cpu_exec_init(env);
    cpu_sh4_reset(env);
    tlb_flush(env, 1);
    return env;
}

static void gen_goto_tb(DisasContext * ctx, int n, target_ulong dest)
{
    TranslationBlock *tb;
    tb = ctx->tb;

    if ((tb->pc & TARGET_PAGE_MASK) == (dest & TARGET_PAGE_MASK) &&
	!ctx->singlestep_enabled) {
	/* Use a direct jump if in same page and singlestep not enabled */
	if (n == 0)
	    gen_op_goto_tb0(TBPARAM(tb));
	else
	    gen_op_goto_tb1(TBPARAM(tb));
	gen_op_movl_imm_T0((long) tb + n);
    } else {
	gen_op_movl_imm_T0(0);
    }
    gen_op_movl_imm_PC(dest);
    if (ctx->singlestep_enabled)
	gen_op_debug();
    gen_op_exit_tb();
}

static void gen_jump(DisasContext * ctx)
{
    if (ctx->delayed_pc == (uint32_t) - 1) {
	/* Target is not statically known, it comes necessarily from a
	   delayed jump as immediate jump are conditinal jumps */
	gen_op_movl_delayed_pc_PC();
	gen_op_movl_imm_T0(0);
	if (ctx->singlestep_enabled)
	    gen_op_debug();
	gen_op_exit_tb();
    } else {
	gen_goto_tb(ctx, 0, ctx->delayed_pc);
    }
}

/* Immediate conditional jump (bt or bf) */
static void gen_conditional_jump(DisasContext * ctx,
				 target_ulong ift, target_ulong ifnott)
{
    int l1;

    l1 = gen_new_label();
    gen_op_jT(l1);
    gen_goto_tb(ctx, 0, ifnott);
    gen_set_label(l1);
    gen_goto_tb(ctx, 1, ift);
}

/* Delayed conditional jump (bt or bf) */
static void gen_delayed_conditional_jump(DisasContext * ctx)
{
    int l1;

    l1 = gen_new_label();
    gen_op_jdelayed(l1);
    gen_goto_tb(ctx, 1, ctx->pc + 2);
    gen_set_label(l1);
    gen_jump(ctx);
}

#define B3_0 (ctx->opcode & 0xf)
#define B6_4 ((ctx->opcode >> 4) & 0x7)
#define B7_4 ((ctx->opcode >> 4) & 0xf)
#define B7_0 (ctx->opcode & 0xff)
#define B7_0s ((int32_t) (int8_t) (ctx->opcode & 0xff))
#define B11_0s (ctx->opcode & 0x800 ? 0xfffff000 | (ctx->opcode & 0xfff) : \
  (ctx->opcode & 0xfff))
#define B11_8 ((ctx->opcode >> 8) & 0xf)
#define B15_12 ((ctx->opcode >> 12) & 0xf)

#define REG(x) ((x) < 8 && (ctx->sr & (SR_MD | SR_RB)) == (SR_MD | SR_RB) ? \
		(x) + 16 : (x))

#define ALTREG(x) ((x) < 8 && (ctx->sr & (SR_MD | SR_RB)) != (SR_MD | SR_RB) \
		? (x) + 16 : (x))

#define FREG(x) (ctx->fpscr & FPSCR_FR ? (x) ^ 0x10 : (x))
#define XHACK(x) ((((x) & 1 ) << 4) | ((x) & 0xe))
#define XREG(x) (ctx->fpscr & FPSCR_FR ? XHACK(x) ^ 0x10 : XHACK(x))
#define DREG(x) FREG(x) /* Assumes lsb of (x) is always 0 */

#define CHECK_NOT_DELAY_SLOT \
  if (ctx->flags & (DELAY_SLOT | DELAY_SLOT_CONDITIONAL)) \
  {gen_op_raise_slot_illegal_instruction (); ctx->bstate = BS_EXCP; \
   return;}

void _decode_opc(DisasContext * ctx)
{
#if 0
    fprintf(stderr, "Translating opcode 0x%04x\n", ctx->opcode);
#endif
    switch (ctx->opcode) {
    case 0x0019:		/* div0u */
	gen_op_div0u();
	return;
    case 0x000b:		/* rts */
	CHECK_NOT_DELAY_SLOT gen_op_rts();
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
    case 0x0028:		/* clrmac */
	gen_op_clrmac();
	return;
    case 0x0048:		/* clrs */
	gen_op_clrs();
	return;
    case 0x0008:		/* clrt */
	gen_op_clrt();
	return;
    case 0x0038:		/* ldtlb */
	assert(0);		/* XXXXX */
	return;
    case 0x002b:		/* rte */
	CHECK_NOT_DELAY_SLOT gen_op_rte();
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
    case 0x0058:		/* sets */
	gen_op_sets();
	return;
    case 0x0018:		/* sett */
	gen_op_sett();
	return;
    case 0xfbfb:		/* frchg */
	gen_op_frchg();
	ctx->bstate = BS_STOP;
	return;
    case 0xf3fb:		/* fschg */
	gen_op_fschg();
	ctx->bstate = BS_STOP;
	return;
    case 0x0009:		/* nop */
	return;
    case 0x001b:		/* sleep */
	assert(0);		/* XXXXX */
	return;
    }

    switch (ctx->opcode & 0xf000) {
    case 0x1000:		/* mov.l Rm,@(disp,Rn) */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_addl_imm_T1(B3_0 * 4);
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x5000:		/* mov.l @(disp,Rm),Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_addl_imm_T0(B3_0 * 4);
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0xe000:		/* mov.l #imm,Rn */
	gen_op_movl_imm_rN(B7_0s, REG(B11_8));
	return;
    case 0x9000:		/* mov.w @(disp,PC),Rn */
	gen_op_movl_imm_T0(ctx->pc + 4 + B7_0 * 2);
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0xd000:		/* mov.l @(disp,PC),Rn */
	gen_op_movl_imm_T0((ctx->pc + 4 + B7_0 * 4) & ~3);
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x7000:		/* add.l #imm,Rn */
	gen_op_add_imm_rN(B7_0s, REG(B11_8));
	return;
    case 0xa000:		/* bra disp */
	CHECK_NOT_DELAY_SLOT
	    gen_op_bra(ctx->delayed_pc = ctx->pc + 4 + B11_0s * 2);
	ctx->flags |= DELAY_SLOT;
	return;
    case 0xb000:		/* bsr disp */
	CHECK_NOT_DELAY_SLOT
	    gen_op_bsr(ctx->pc + 4, ctx->delayed_pc =
		       ctx->pc + 4 + B11_0s * 2);
	ctx->flags |= DELAY_SLOT;
	return;
    }

    switch (ctx->opcode & 0xf00f) {
    case 0x6003:		/* mov Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x2000:		/* mov.b Rm,@Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stb_T0_T1(ctx);
	return;
    case 0x2001:		/* mov.w Rm,@Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stw_T0_T1(ctx);
	return;
    case 0x2002:		/* mov.l Rm,@Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x6000:		/* mov.b @Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldb_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x6001:		/* mov.w @Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x6002:		/* mov.l @Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x2004:		/* mov.b Rm,@-Rn */
	gen_op_dec1_rN(REG(B11_8));
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stb_T0_T1(ctx);
	return;
    case 0x2005:		/* mov.w Rm,@-Rn */
	gen_op_dec2_rN(REG(B11_8));
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stw_T0_T1(ctx);
	return;
    case 0x2006:		/* mov.l Rm,@-Rn */
	gen_op_dec4_rN(REG(B11_8));
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x6004:		/* mov.b @Rm+,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldb_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	gen_op_inc1_rN(REG(B7_4));
	return;
    case 0x6005:		/* mov.w @Rm+,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	gen_op_inc2_rN(REG(B7_4));
	return;
    case 0x6006:		/* mov.l @Rm+,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	gen_op_inc4_rN(REG(B7_4));
	return;
    case 0x0004:		/* mov.b Rm,@(R0,Rn) */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_add_rN_T1(REG(0));
	gen_op_stb_T0_T1(ctx);
	return;
    case 0x0005:		/* mov.w Rm,@(R0,Rn) */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_add_rN_T1(REG(0));
	gen_op_stw_T0_T1(ctx);
	return;
    case 0x0006:		/* mov.l Rm,@(R0,Rn) */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_add_rN_T1(REG(0));
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x000c:		/* mov.b @(R0,Rm),Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_add_rN_T0(REG(0));
	gen_op_ldb_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x000d:		/* mov.w @(R0,Rm),Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_add_rN_T0(REG(0));
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x000e:		/* mov.l @(R0,Rm),Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_add_rN_T0(REG(0));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x6008:		/* swap.b Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_swapb_T0();
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x6009:		/* swap.w Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_swapw_T0();
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x200d:		/* xtrct Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_xtrct_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x300c:		/* add Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_add_T0_rN(REG(B11_8));
	return;
    case 0x300e:		/* addc Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_addc_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x300f:		/* addv Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_addv_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x2009:		/* and Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_and_T0_rN(REG(B11_8));
	return;
    case 0x3000:		/* cmp/eq Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_eq_T0_T1();
	return;
    case 0x3003:		/* cmp/ge Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_ge_T0_T1();
	return;
    case 0x3007:		/* cmp/gt Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_gt_T0_T1();
	return;
    case 0x3006:		/* cmp/hi Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_hi_T0_T1();
	return;
    case 0x3002:		/* cmp/hs Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_hs_T0_T1();
	return;
    case 0x200c:		/* cmp/str Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_cmp_str_T0_T1();
	return;
    case 0x2007:		/* div0s Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_div0s_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x3004:		/* div1 Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_div1_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x300d:		/* dmuls.l Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_dmulsl_T0_T1();
	return;
    case 0x3005:		/* dmulu.l Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_dmulul_T0_T1();
	return;
    case 0x600e:		/* exts.b Rm,Rn */
	gen_op_movb_rN_T0(REG(B7_4));
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x600f:		/* exts.w Rm,Rn */
	gen_op_movw_rN_T0(REG(B7_4));
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x600c:		/* extu.b Rm,Rn */
	gen_op_movub_rN_T0(REG(B7_4));
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x600d:		/* extu.w Rm,Rn */
	gen_op_movuw_rN_T0(REG(B7_4));
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x000f:		/* mac.l @Rm+,@Rn- */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_T1();
	gen_op_movl_rN_T1(REG(B7_4));
	gen_op_ldl_T0_T0(ctx);
	gen_op_macl_T0_T1();
	gen_op_inc4_rN(REG(B7_4));
	gen_op_inc4_rN(REG(B11_8));
	return;
    case 0x400f:		/* mac.w @Rm+,@Rn+ */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_T1();
	gen_op_movl_rN_T1(REG(B7_4));
	gen_op_ldl_T0_T0(ctx);
	gen_op_macw_T0_T1();
	gen_op_inc2_rN(REG(B7_4));
	gen_op_inc2_rN(REG(B11_8));
	return;
    case 0x0007:		/* mul.l Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_mull_T0_T1();
	return;
    case 0x200f:		/* muls.w Rm,Rn */
	gen_op_movw_rN_T0(REG(B7_4));
	gen_op_movw_rN_T1(REG(B11_8));
	gen_op_mulsw_T0_T1();
	return;
    case 0x200e:		/* mulu.w Rm,Rn */
	gen_op_movuw_rN_T0(REG(B7_4));
	gen_op_movuw_rN_T1(REG(B11_8));
	gen_op_muluw_T0_T1();
	return;
    case 0x600b:		/* neg Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_neg_T0();
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x600a:		/* negc Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_negc_T0();
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x6007:		/* not Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_not_T0();
	gen_op_movl_T0_rN(REG(B11_8));
	return;
    case 0x200b:		/* or Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_or_T0_rN(REG(B11_8));
	return;
    case 0x400c:		/* shad Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_shad_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x400d:		/* shld Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_shld_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x3008:		/* sub Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_sub_T0_rN(REG(B11_8));
	return;
    case 0x300a:		/* subc Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_subc_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x300b:		/* subv Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_subv_T0_T1();
	gen_op_movl_T1_rN(REG(B11_8));
	return;
    case 0x2008:		/* tst Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_tst_T0_T1();
	return;
    case 0x200a:		/* xor Rm,Rn */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_xor_T0_rN(REG(B11_8));
	return;
    case 0xf00c: /* fmov {F,D,X}Rm,{F,D,X}Rn - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0110)
		break; /* illegal instruction */
	    gen_op_fmov_drN_DT0(DREG(B7_4));
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	} else {
	    gen_op_fmov_frN_FT0(FREG(B7_4));
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	}
	return;
    case 0xf00a: /* fmov {F,D,X}Rm,@Rn - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0010)
		break; /* illegal instruction */
	    gen_op_fmov_drN_DT0(DREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_stfq_DT0_T1(ctx);
	} else {
	    gen_op_fmov_frN_FT0(FREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_stfl_FT0_T1(ctx);
	}
	return;
    case 0xf008: /* fmov @Rm,{F,D,X}Rn - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_ldfq_T0_DT0(ctx);
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	} else {
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_ldfl_T0_FT0(ctx);
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	}
	return;
    case 0xf009: /* fmov @Rm+,{F,D,X}Rn - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_ldfq_T0_DT0(ctx);
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	    gen_op_inc8_rN(REG(B7_4));
	} else {
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_ldfl_T0_FT0(ctx);
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	    gen_op_inc4_rN(REG(B7_4));
	}
	return;
    case 0xf00b: /* fmov {F,D,X}Rm,@-Rn - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_dec8_rN(REG(B11_8));
	    gen_op_fmov_drN_DT0(DREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_stfq_DT0_T1(ctx);
	} else {
	    gen_op_dec4_rN(REG(B11_8));
	    gen_op_fmov_frN_FT0(FREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_stfl_FT0_T1(ctx);
	}
	return;
    case 0xf006: /* fmov @(R0,Rm),{F,D,X}Rm - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_add_rN_T0(REG(0));
	    gen_op_ldfq_T0_DT0(ctx);
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	} else {
	    gen_op_movl_rN_T0(REG(B7_4));
	    gen_op_add_rN_T0(REG(0));
	    gen_op_ldfl_T0_FT0(ctx);
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	}
	return;
    case 0xf007: /* fmov {F,D,X}Rn,@(R0,Rn) - FPSCR: Nothing */
	if (ctx->fpscr & FPSCR_SZ) {
	    if (ctx->opcode & 0x0010)
		break; /* illegal instruction */
	    gen_op_fmov_drN_DT0(DREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_add_rN_T1(REG(0));
	    gen_op_stfq_DT0_T1(ctx);
	} else {
	    gen_op_fmov_frN_FT0(FREG(B7_4));
	    gen_op_movl_rN_T1(REG(B11_8));
	    gen_op_add_rN_T1(REG(0));
	    gen_op_stfl_FT0_T1(ctx);
	}
	return;
    case 0xf000: /* fadd Rm,Rn - FPSCR: R[PR,Enable.O/U/I]/W[Cause,Flag] */
    case 0xf001: /* fsub Rm,Rn - FPSCR: R[PR,Enable.O/U/I]/W[Cause,Flag] */
    case 0xf002: /* fmul Rm,Rn - FPSCR: R[PR,Enable.O/U/I]/W[Cause,Flag] */
    case 0xf003: /* fdiv Rm,Rn - FPSCR: R[PR,Enable.O/U/I]/W[Cause,Flag] */
    case 0xf004: /* fcmp/eq Rm,Rn - FPSCR: R[PR,Enable.V]/W[Cause,Flag] */
    case 0xf005: /* fcmp/gt Rm,Rn - FPSCR: R[PR,Enable.V]/W[Cause,Flag] */
	if (ctx->fpscr & FPSCR_PR) {
	    if (ctx->opcode & 0x0110)
		break; /* illegal instruction */
	    gen_op_fmov_drN_DT1(DREG(B7_4));
	    gen_op_fmov_drN_DT0(DREG(B11_8));
	}
	else {
	    gen_op_fmov_frN_FT1(FREG(B7_4));
	    gen_op_fmov_frN_FT0(FREG(B11_8));
	}

	switch (ctx->opcode & 0xf00f) {
	case 0xf000:		/* fadd Rm,Rn */
	    ctx->fpscr & FPSCR_PR ? gen_op_fadd_DT() : gen_op_fadd_FT();
	    break;
	case 0xf001:		/* fsub Rm,Rn */
	    ctx->fpscr & FPSCR_PR ? gen_op_fsub_DT() : gen_op_fsub_FT();
	    break;
	case 0xf002:		/* fmul Rm,Rn */
	    ctx->fpscr & FPSCR_PR ? gen_op_fmul_DT() : gen_op_fmul_FT();
	    break;
	case 0xf003:		/* fdiv Rm,Rn */
	    ctx->fpscr & FPSCR_PR ? gen_op_fdiv_DT() : gen_op_fdiv_FT();
	    break;
	case 0xf004:		/* fcmp/eq Rm,Rn */
	    return;
	case 0xf005:		/* fcmp/gt Rm,Rn */
	    return;
	}

	if (ctx->fpscr & FPSCR_PR) {
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	}
	else {
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	}
	return;
    }

    switch (ctx->opcode & 0xff00) {
    case 0xc900:		/* and #imm,R0 */
	gen_op_and_imm_rN(B7_0, REG(0));
	return;
    case 0xcd00:		/* and.b #imm,@(R0+GBR) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_addl_GBR_T0();
	gen_op_movl_T0_T1();
	gen_op_ldb_T0_T0(ctx);
	gen_op_and_imm_T0(B7_0);
	gen_op_stb_T0_T1(ctx);
	return;
    case 0x8b00:		/* bf label */
	CHECK_NOT_DELAY_SLOT
	    gen_conditional_jump(ctx, ctx->pc + 2,
				 ctx->pc + 4 + B7_0s * 2);
	ctx->bstate = BS_BRANCH;
	return;
    case 0x8f00:		/* bf/s label */
	CHECK_NOT_DELAY_SLOT
	    gen_op_bf_s(ctx->delayed_pc = ctx->pc + 4 + B7_0s * 2);
	ctx->flags |= DELAY_SLOT_CONDITIONAL;
	return;
    case 0x8900:		/* bt label */
	CHECK_NOT_DELAY_SLOT
	    gen_conditional_jump(ctx, ctx->pc + 4 + B7_0s * 2,
				 ctx->pc + 2);
	ctx->bstate = BS_BRANCH;
	return;
    case 0x8d00:		/* bt/s label */
	CHECK_NOT_DELAY_SLOT
	    gen_op_bt_s(ctx->delayed_pc = ctx->pc + 4 + B7_0s * 2);
	ctx->flags |= DELAY_SLOT_CONDITIONAL;
	return;
    case 0x8800:		/* cmp/eq #imm,R0 */
	gen_op_movl_rN_T0(REG(0));
	gen_op_cmp_eq_imm_T0(B7_0s);
	return;
    case 0xc400:		/* mov.b @(disp,GBR),R0 */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_ldb_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(0));
	return;
    case 0xc500:		/* mov.w @(disp,GBR),R0 */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(0));
	return;
    case 0xc600:		/* mov.l @(disp,GBR),R0 */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(0));
	return;
    case 0xc000:		/* mov.b R0,@(disp,GBR) */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_movl_T0_T1();
	gen_op_movl_rN_T0(REG(0));
	gen_op_stb_T0_T1(ctx);
	return;
    case 0xc100:		/* mov.w R0,@(disp,GBR) */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_movl_T0_T1();
	gen_op_movl_rN_T0(REG(0));
	gen_op_stw_T0_T1(ctx);
	return;
    case 0xc200:		/* mov.l R0,@(disp,GBR) */
	gen_op_stc_gbr_T0();
	gen_op_addl_imm_T0(B7_0);
	gen_op_movl_T0_T1();
	gen_op_movl_rN_T0(REG(0));
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x8000:		/* mov.b R0,@(disp,Rn) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_movl_rN_T1(REG(B7_4));
	gen_op_addl_imm_T1(B3_0);
	gen_op_stb_T0_T1(ctx);
	return;
    case 0x8100:		/* mov.w R0,@(disp,Rn) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_movl_rN_T1(REG(B7_4));
	gen_op_addl_imm_T1(B3_0 * 2);
	gen_op_stw_T0_T1(ctx);
	return;
    case 0x8400:		/* mov.b @(disp,Rn),R0 */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_addl_imm_T0(B3_0);
	gen_op_ldb_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(0));
	return;
    case 0x8500:		/* mov.w @(disp,Rn),R0 */
	gen_op_movl_rN_T0(REG(B7_4));
	gen_op_addl_imm_T0(B3_0 * 2);
	gen_op_ldw_T0_T0(ctx);
	gen_op_movl_T0_rN(REG(0));
	return;
    case 0xc700:		/* mova @(disp,PC),R0 */
	gen_op_movl_imm_rN(((ctx->pc & 0xfffffffc) + 4 + B7_0 * 4) & ~3,
			   REG(0));
	return;
    case 0xcb00:		/* or #imm,R0 */
	gen_op_or_imm_rN(B7_0, REG(0));
	return;
    case 0xcf00:		/* or.b #imm,@(R0+GBR) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_addl_GBR_T0();
	gen_op_movl_T0_T1();
	gen_op_ldb_T0_T0(ctx);
	gen_op_or_imm_T0(B7_0);
	gen_op_stb_T0_T1(ctx);
	return;
    case 0xc300:		/* trapa #imm */
	CHECK_NOT_DELAY_SLOT gen_op_movl_imm_PC(ctx->pc);
	gen_op_trapa(B7_0);
	ctx->bstate = BS_BRANCH;
	return;
    case 0xc800:		/* tst #imm,R0 */
	gen_op_tst_imm_rN(B7_0, REG(0));
	return;
    case 0xcc00:		/* tst #imm,@(R0+GBR) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_addl_GBR_T0();
	gen_op_ldb_T0_T0(ctx);
	gen_op_tst_imm_T0(B7_0);
	return;
    case 0xca00:		/* xor #imm,R0 */
	gen_op_xor_imm_rN(B7_0, REG(0));
	return;
    case 0xce00:		/* xor.b #imm,@(R0+GBR) */
	gen_op_movl_rN_T0(REG(0));
	gen_op_addl_GBR_T0();
	gen_op_movl_T0_T1();
	gen_op_ldb_T0_T0(ctx);
	gen_op_xor_imm_T0(B7_0);
	gen_op_stb_T0_T1(ctx);
	return;
    }

    switch (ctx->opcode & 0xf08f) {
    case 0x408e:		/* ldc Rm,Rn_BANK */
	gen_op_movl_rN_rN(REG(B11_8), ALTREG(B6_4));
	return;
    case 0x4087:		/* ldc.l @Rm+,Rn_BANK */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	gen_op_movl_T0_rN(ALTREG(B6_4));
	gen_op_inc4_rN(REG(B11_8));
	return;
    case 0x0082:		/* stc Rm_BANK,Rn */
	gen_op_movl_rN_rN(ALTREG(B6_4), REG(B11_8));
	return;
    case 0x4083:		/* stc.l Rm_BANK,@-Rn */
	gen_op_dec4_rN(REG(B11_8));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_movl_rN_T0(ALTREG(B6_4));
	gen_op_stl_T0_T1(ctx);
	return;
    }

    switch (ctx->opcode & 0xf0ff) {
    case 0x0023:		/* braf Rn */
	CHECK_NOT_DELAY_SLOT gen_op_movl_rN_T0(REG(B11_8));
	gen_op_braf_T0(ctx->pc + 4);
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
    case 0x0003:		/* bsrf Rn */
	CHECK_NOT_DELAY_SLOT gen_op_movl_rN_T0(REG(B11_8));
	gen_op_bsrf_T0(ctx->pc + 4);
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
    case 0x4015:		/* cmp/pl Rn */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_cmp_pl_T0();
	return;
    case 0x4011:		/* cmp/pz Rn */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_cmp_pz_T0();
	return;
    case 0x4010:		/* dt Rn */
	gen_op_dt_rN(REG(B11_8));
	return;
    case 0x402b:		/* jmp @Rn */
	CHECK_NOT_DELAY_SLOT gen_op_movl_rN_T0(REG(B11_8));
	gen_op_jmp_T0();
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
    case 0x400b:		/* jsr @Rn */
	CHECK_NOT_DELAY_SLOT gen_op_movl_rN_T0(REG(B11_8));
	gen_op_jsr_T0(ctx->pc + 4);
	ctx->flags |= DELAY_SLOT;
	ctx->delayed_pc = (uint32_t) - 1;
	return;
#define LDST(reg,ldnum,ldpnum,ldop,stnum,stpnum,stop,extrald)	\
  case ldnum:							\
    gen_op_movl_rN_T0 (REG(B11_8));				\
    gen_op_##ldop##_T0_##reg ();				\
    extrald							\
    return;							\
  case ldpnum:							\
    gen_op_movl_rN_T0 (REG(B11_8));				\
    gen_op_ldl_T0_T0 (ctx);					\
    gen_op_inc4_rN (REG(B11_8));				\
    gen_op_##ldop##_T0_##reg ();				\
    extrald							\
    return;							\
  case stnum:							\
    gen_op_##stop##_##reg##_T0 ();					\
    gen_op_movl_T0_rN (REG(B11_8));				\
    return;							\
  case stpnum:							\
    gen_op_##stop##_##reg##_T0 ();				\
    gen_op_dec4_rN (REG(B11_8));				\
    gen_op_movl_rN_T1 (REG(B11_8));				\
    gen_op_stl_T0_T1 (ctx);					\
    return;
	LDST(sr, 0x400e, 0x4007, ldc, 0x0002, 0x4003, stc, ctx->bstate =
	     BS_STOP;)
	LDST(gbr, 0x401e, 0x4017, ldc, 0x0012, 0x4013, stc,)
	LDST(vbr, 0x402e, 0x4027, ldc, 0x0022, 0x4023, stc,)
	LDST(ssr, 0x403e, 0x4037, ldc, 0x0032, 0x4033, stc,)
	LDST(spc, 0x404e, 0x4047, ldc, 0x0042, 0x4043, stc,)
	LDST(dbr, 0x40fa, 0x40f6, ldc, 0x00fa, 0x40f2, stc,)
	LDST(mach, 0x400a, 0x4006, lds, 0x000a, 0x4002, sts,)
	LDST(macl, 0x401a, 0x4016, lds, 0x001a, 0x4012, sts,)
	LDST(pr, 0x402a, 0x4026, lds, 0x002a, 0x4022, sts,)
	LDST(fpul, 0x405a, 0x4056, lds, 0x005a, 0x4052, sts,)
	LDST(fpscr, 0x406a, 0x4066, lds, 0x006a, 0x4062, sts, ctx->bstate =
	     BS_STOP;)
    case 0x00c3:		/* movca.l R0,@Rm */
	gen_op_movl_rN_T0(REG(0));
	gen_op_movl_rN_T1(REG(B11_8));
	gen_op_stl_T0_T1(ctx);
	return;
    case 0x0029:		/* movt Rn */
	gen_op_movt_rN(REG(B11_8));
	return;
    case 0x0093:		/* ocbi @Rn */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	return;
    case 0x00a2:		/* ocbp @Rn */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	return;
    case 0x00b3:		/* ocbwb @Rn */
	gen_op_movl_rN_T0(REG(B11_8));
	gen_op_ldl_T0_T0(ctx);
	return;
    case 0x0083:		/* pref @Rn */
	return;
    case 0x4024:		/* rotcl Rn */
	gen_op_rotcl_Rn(REG(B11_8));
	return;
    case 0x4025:		/* rotcr Rn */
	gen_op_rotcr_Rn(REG(B11_8));
	return;
    case 0x4004:		/* rotl Rn */
	gen_op_rotl_Rn(REG(B11_8));
	return;
    case 0x4005:		/* rotr Rn */
	gen_op_rotr_Rn(REG(B11_8));
	return;
    case 0x4000:		/* shll Rn */
    case 0x4020:		/* shal Rn */
	gen_op_shal_Rn(REG(B11_8));
	return;
    case 0x4021:		/* shar Rn */
	gen_op_shar_Rn(REG(B11_8));
	return;
    case 0x4001:		/* shlr Rn */
	gen_op_shlr_Rn(REG(B11_8));
	return;
    case 0x4008:		/* shll2 Rn */
	gen_op_shll2_Rn(REG(B11_8));
	return;
    case 0x4018:		/* shll8 Rn */
	gen_op_shll8_Rn(REG(B11_8));
	return;
    case 0x4028:		/* shll16 Rn */
	gen_op_shll16_Rn(REG(B11_8));
	return;
    case 0x4009:		/* shlr2 Rn */
	gen_op_shlr2_Rn(REG(B11_8));
	return;
    case 0x4019:		/* shlr8 Rn */
	gen_op_shlr8_Rn(REG(B11_8));
	return;
    case 0x4029:		/* shlr16 Rn */
	gen_op_shlr16_Rn(REG(B11_8));
	return;
    case 0x401b:		/* tas.b @Rn */
	gen_op_tasb_rN(REG(B11_8));
	return;
    case 0xf00d: /* fsts FPUL,FRn - FPSCR: Nothing */
	gen_op_movl_fpul_FT0();
	gen_op_fmov_FT0_frN(FREG(B11_8));
	return;
    case 0xf01d: /* flds FRm,FPUL - FPSCR: Nothing */
	gen_op_fmov_frN_FT0(FREG(B11_8));
	gen_op_movl_FT0_fpul();
	return;
    case 0xf02d: /* float FPUL,FRn/DRn - FPSCR: R[PR,Enable.I]/W[Cause,Flag] */
	if (ctx->fpscr & FPSCR_PR) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_float_DT();
	    gen_op_fmov_DT0_drN(DREG(B11_8));
	}
	else {
	    gen_op_float_FT();
	    gen_op_fmov_FT0_frN(FREG(B11_8));
	}
	return;
    case 0xf03d: /* ftrc FRm/DRm,FPUL - FPSCR: R[PR,Enable.V]/W[Cause,Flag] */
	if (ctx->fpscr & FPSCR_PR) {
	    if (ctx->opcode & 0x0100)
		break; /* illegal instruction */
	    gen_op_fmov_drN_DT0(DREG(B11_8));
	    gen_op_ftrc_DT();
	}
	else {
	    gen_op_fmov_frN_FT0(FREG(B11_8));
	    gen_op_ftrc_FT();
	}
	return;
    case 0xf08d: /* fldi0 FRn - FPSCR: R[PR] */
	if (!(ctx->fpscr & FPSCR_PR)) {
	    gen_op_movl_imm_T0(0);
	    gen_op_fmov_T0_frN(FREG(B11_8));
	    return;
	}
	break;
    case 0xf09d: /* fldi1 FRn - FPSCR: R[PR] */
	if (!(ctx->fpscr & FPSCR_PR)) {
	    gen_op_movl_imm_T0(0x3f800000);
	    gen_op_fmov_T0_frN(FREG(B11_8));
	    return;
	}
	break;
    }

    fprintf(stderr, "unknown instruction 0x%04x at pc 0x%08x\n",
	    ctx->opcode, ctx->pc);
    gen_op_raise_illegal_instruction();
    ctx->bstate = BS_EXCP;
}

void decode_opc(DisasContext * ctx)
{
    uint32_t old_flags = ctx->flags;

    _decode_opc(ctx);

    if (old_flags & (DELAY_SLOT | DELAY_SLOT_CONDITIONAL)) {
        if (ctx->flags & DELAY_SLOT_CLEARME) {
            gen_op_store_flags(0);
        }
        ctx->flags = 0;
        ctx->bstate = BS_BRANCH;
        if (old_flags & DELAY_SLOT_CONDITIONAL) {
	    gen_delayed_conditional_jump(ctx);
        } else if (old_flags & DELAY_SLOT) {
            gen_jump(ctx);
	}

    }
}

static inline int
gen_intermediate_code_internal(CPUState * env, TranslationBlock * tb,
                               int search_pc)
{
    DisasContext ctx;
    target_ulong pc_start;
    static uint16_t *gen_opc_end;
    int i, ii;

    pc_start = tb->pc;
    gen_opc_ptr = gen_opc_buf;
    gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;
    gen_opparam_ptr = gen_opparam_buf;
    ctx.pc = pc_start;
    ctx.flags = (uint32_t)tb->flags;
    ctx.bstate = BS_NONE;
    ctx.sr = env->sr;
    ctx.fpscr = env->fpscr;
    ctx.memidx = (env->sr & SR_MD) ? 1 : 0;
    /* We don't know if the delayed pc came from a dynamic or static branch,
       so assume it is a dynamic branch.  */
    ctx.delayed_pc = -1; /* use delayed pc from env pointer */
    ctx.tb = tb;
    ctx.singlestep_enabled = env->singlestep_enabled;
    nb_gen_labels = 0;

#ifdef DEBUG_DISAS
    if (loglevel & CPU_LOG_TB_CPU) {
	fprintf(logfile,
		"------------------------------------------------\n");
	cpu_dump_state(env, logfile, fprintf, 0);
    }
#endif

    ii = -1;
    while (ctx.bstate == BS_NONE && gen_opc_ptr < gen_opc_end) {
	if (env->nb_breakpoints > 0) {
	    for (i = 0; i < env->nb_breakpoints; i++) {
		if (ctx.pc == env->breakpoints[i]) {
		    /* We have hit a breakpoint - make sure PC is up-to-date */
		    gen_op_movl_imm_PC(ctx.pc);
		    gen_op_debug();
		    ctx.bstate = BS_EXCP;
		    break;
		}
	    }
	}
        if (search_pc) {
            i = gen_opc_ptr - gen_opc_buf;
            if (ii < i) {
                ii++;
                while (ii < i)
                    gen_opc_instr_start[ii++] = 0;
            }
            gen_opc_pc[ii] = ctx.pc;
            gen_opc_hflags[ii] = ctx.flags;
            gen_opc_instr_start[ii] = 1;
        }
#if 0
	fprintf(stderr, "Loading opcode at address 0x%08x\n", ctx.pc);
	fflush(stderr);
#endif
	ctx.opcode = lduw_code(ctx.pc);
	decode_opc(&ctx);
	ctx.pc += 2;
	if ((ctx.pc & (TARGET_PAGE_SIZE - 1)) == 0)
	    break;
	if (env->singlestep_enabled)
	    break;
#ifdef SH4_SINGLE_STEP
	break;
#endif
    }
    if (env->singlestep_enabled) {
        gen_op_debug();
    } else {
	switch (ctx.bstate) {
        case BS_STOP:
            /* gen_op_interrupt_restart(); */
            /* fall through */
        case BS_NONE:
            if (ctx.flags) {
                gen_op_store_flags(ctx.flags | DELAY_SLOT_CLEARME);
	    }
            gen_goto_tb(&ctx, 0, ctx.pc);
            break;
        case BS_EXCP:
            /* gen_op_interrupt_restart(); */
            gen_op_movl_imm_T0(0);
            gen_op_exit_tb();
            break;
        case BS_BRANCH:
        default:
            break;
	}
    }

    *gen_opc_ptr = INDEX_op_end;
    if (search_pc) {
        i = gen_opc_ptr - gen_opc_buf;
        ii++;
        while (ii <= i)
            gen_opc_instr_start[ii++] = 0;
    } else {
        tb->size = ctx.pc - pc_start;
    }

#ifdef DEBUG_DISAS
#ifdef SH4_DEBUG_DISAS
    if (loglevel & CPU_LOG_TB_IN_ASM)
	fprintf(logfile, "\n");
#endif
    if (loglevel & CPU_LOG_TB_IN_ASM) {
	fprintf(logfile, "IN:\n");	/* , lookup_symbol(pc_start)); */
	target_disas(logfile, pc_start, ctx.pc - pc_start, 0);
	fprintf(logfile, "\n");
    }
    if (loglevel & CPU_LOG_TB_OP) {
	fprintf(logfile, "OP:\n");
	dump_ops(gen_opc_buf, gen_opparam_buf);
	fprintf(logfile, "\n");
    }
#endif
    return 0;
}

int gen_intermediate_code(CPUState * env, struct TranslationBlock *tb)
{
    return gen_intermediate_code_internal(env, tb, 0);
}

int gen_intermediate_code_pc(CPUState * env, struct TranslationBlock *tb)
{
    return gen_intermediate_code_internal(env, tb, 1);
}
