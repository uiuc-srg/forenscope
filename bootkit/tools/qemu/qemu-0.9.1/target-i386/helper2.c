/*
 *  i386 helpers (without register variable usage)
 *
 *  Copyright (c) 2003 Fabrice Bellard
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
#include <signal.h>
#include <assert.h>

#include "cpu.h"
#include "exec-all.h"
#include "svm.h"

//#define DEBUG_MMU

static int cpu_x86_register (CPUX86State *env, const char *cpu_model);

static void add_flagname_to_bitmaps(char *flagname, uint32_t *features, 
                                    uint32_t *ext_features, 
                                    uint32_t *ext2_features, 
                                    uint32_t *ext3_features)
{
    int i;
    /* feature flags taken from "Intel Processor Identification and the CPUID
     * Instruction" and AMD's "CPUID Specification". In cases of disagreement 
     * about feature names, the Linux name is used. */
    const char *feature_name[] = {
        "fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
        "cx8", "apic", NULL, "sep", "mtrr", "pge", "mca", "cmov",
        "pat", "pse36", "pn" /* Intel psn */, "clflush" /* Intel clfsh */, NULL, "ds" /* Intel dts */, "acpi", "mmx",
        "fxsr", "sse", "sse2", "ss", "ht" /* Intel htt */, "tm", "ia64", "pbe",
    };
    const char *ext_feature_name[] = {
       "pni" /* Intel,AMD sse3 */, NULL, NULL, "monitor", "ds_cpl", "vmx", NULL /* Linux smx */, "est",
       "tm2", "ssse3", "cid", NULL, NULL, "cx16", "xtpr", NULL,
       NULL, NULL, "dca", NULL, NULL, NULL, NULL, "popcnt",
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    };
    const char *ext2_feature_name[] = {
       "fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
       "cx8" /* AMD CMPXCHG8B */, "apic", NULL, "syscall", "mttr", "pge", "mca", "cmov",
       "pat", "pse36", NULL, NULL /* Linux mp */, "nx" /* Intel xd */, NULL, "mmxext", "mmx",
       "fxsr", "fxsr_opt" /* AMD ffxsr */, "pdpe1gb" /* AMD Page1GB */, "rdtscp", NULL, "lm" /* Intel 64 */, "3dnowext", "3dnow",
    };
    const char *ext3_feature_name[] = {
       "lahf_lm" /* AMD LahfSahf */, "cmp_legacy", "svm", "extapic" /* AMD ExtApicSpace */, "cr8legacy" /* AMD AltMovCr8 */, "abm", "sse4a", "misalignsse",
       "3dnowprefetch", "osvw", NULL /* Linux ibs */, NULL, "skinit", "wdt", NULL, NULL,
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    };

    for ( i = 0 ; i < 32 ; i++ ) 
        if (feature_name[i] && !strcmp (flagname, feature_name[i])) {
            *features |= 1 << i;
            return;
        }
    for ( i = 0 ; i < 32 ; i++ ) 
        if (ext_feature_name[i] && !strcmp (flagname, ext_feature_name[i])) {
            *ext_features |= 1 << i;
            return;
        }
    for ( i = 0 ; i < 32 ; i++ ) 
        if (ext2_feature_name[i] && !strcmp (flagname, ext2_feature_name[i])) {
            *ext2_features |= 1 << i;
            return;
        }
    for ( i = 0 ; i < 32 ; i++ ) 
        if (ext3_feature_name[i] && !strcmp (flagname, ext3_feature_name[i])) {
            *ext3_features |= 1 << i;
            return;
        }
    fprintf(stderr, "CPU feature %s not found\n", flagname);
}

CPUX86State *cpu_x86_init(const char *cpu_model)
{
    CPUX86State *env;
    static int inited;

    env = qemu_mallocz(sizeof(CPUX86State));
    if (!env)
        return NULL;
    cpu_exec_init(env);
    env->cpu_model_str = cpu_model;

    /* init various static tables */
    if (!inited) {
        inited = 1;
        optimize_flags_init();
    }
    if (cpu_x86_register(env, cpu_model) < 0) {
        cpu_x86_close(env);
        return NULL;
    }
    cpu_reset(env);
#ifdef USE_KQEMU
    kqemu_init(env);
#endif
    return env;
}

typedef struct x86_def_t {
    const char *name;
    uint32_t level;
    uint32_t vendor1, vendor2, vendor3;
    int family;
    int model;
    int stepping;
    uint32_t features, ext_features, ext2_features, ext3_features;
    uint32_t xlevel;
} x86_def_t;

#define PPRO_FEATURES (CPUID_FP87 | CPUID_DE | CPUID_PSE | CPUID_TSC | \
          CPUID_MSR | CPUID_MCE | CPUID_CX8 | CPUID_PGE | CPUID_CMOV | \
          CPUID_PAT | CPUID_FXSR | CPUID_MMX | CPUID_SSE | CPUID_SSE2 | \
          CPUID_PAE | CPUID_SEP | CPUID_APIC)
static x86_def_t x86_defs[] = {
#ifdef TARGET_X86_64
    {
        .name = "qemu64",
        .level = 2,
        .vendor1 = 0x68747541, /* "Auth" */
        .vendor2 = 0x69746e65, /* "enti" */
        .vendor3 = 0x444d4163, /* "cAMD" */
        .family = 6,
        .model = 2,
        .stepping = 3,
        .features = PPRO_FEATURES | 
        /* these features are needed for Win64 and aren't fully implemented */
            CPUID_MTRR | CPUID_CLFLUSH | CPUID_MCA |
        /* this feature is needed for Solaris and isn't fully implemented */
            CPUID_PSE36,
        .ext_features = CPUID_EXT_SSE3,
        .ext2_features = (PPRO_FEATURES & 0x0183F3FF) | 
            CPUID_EXT2_LM | CPUID_EXT2_SYSCALL | CPUID_EXT2_NX,
        .ext3_features = CPUID_EXT3_SVM,
        .xlevel = 0x8000000A,
    },
#endif
    {
        .name = "qemu32",
        .level = 2,
        .family = 6,
        .model = 3,
        .stepping = 3,
        .features = PPRO_FEATURES,
        .ext_features = CPUID_EXT_SSE3,
        .xlevel = 0,
    },
    {
        .name = "486",
        .level = 0,
        .family = 4,
        .model = 0,
        .stepping = 0,
        .features = 0x0000000B,
        .xlevel = 0,
    },
    {
        .name = "pentium",
        .level = 1,
        .family = 5,
        .model = 4,
        .stepping = 3,
        .features = 0x008001BF,
        .xlevel = 0,
    },
    {
        .name = "pentium2",
        .level = 2,
        .family = 6,
        .model = 5,
        .stepping = 2,
        .features = 0x0183F9FF,
        .xlevel = 0,
    },
    {
        .name = "pentium3",
        .level = 2,
        .family = 6,
        .model = 7,
        .stepping = 3,
        .features = 0x0383F9FF,
        .xlevel = 0,
    },
};

static int cpu_x86_find_by_name(x86_def_t *x86_cpu_def, const char *cpu_model)
{
    unsigned int i;
    x86_def_t *def;

    char *s = strdup(cpu_model);
    char *featurestr, *name = strtok(s, ",");
    uint32_t plus_features = 0, plus_ext_features = 0, plus_ext2_features = 0, plus_ext3_features = 0;
    uint32_t minus_features = 0, minus_ext_features = 0, minus_ext2_features = 0, minus_ext3_features = 0;
    int family = -1, model = -1, stepping = -1;

    def = NULL;
    for (i = 0; i < sizeof(x86_defs) / sizeof(x86_def_t); i++) {
        if (strcmp(name, x86_defs[i].name) == 0) {
            def = &x86_defs[i];
            break;
        }
    }
    if (!def)
        goto error;
    memcpy(x86_cpu_def, def, sizeof(*def));

    featurestr = strtok(NULL, ",");

    while (featurestr) {
        char *val;
        if (featurestr[0] == '+') {
            add_flagname_to_bitmaps(featurestr + 1, &plus_features, &plus_ext_features, &plus_ext2_features, &plus_ext3_features);
        } else if (featurestr[0] == '-') {
            add_flagname_to_bitmaps(featurestr + 1, &minus_features, &minus_ext_features, &minus_ext2_features, &minus_ext3_features);
        } else if ((val = strchr(featurestr, '='))) {
            *val = 0; val++;
            if (!strcmp(featurestr, "family")) {
                char *err;
                family = strtol(val, &err, 10);
                if (!*val || *err || family < 0) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    x86_cpu_def = 0;
                    goto error;
                }
                x86_cpu_def->family = family;
            } else if (!strcmp(featurestr, "model")) {
                char *err;
                model = strtol(val, &err, 10);
                if (!*val || *err || model < 0 || model > 0xf) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    x86_cpu_def = 0;
                    goto error;
                }
                x86_cpu_def->model = model;
            } else if (!strcmp(featurestr, "stepping")) {
                char *err;
                stepping = strtol(val, &err, 10);
                if (!*val || *err || stepping < 0 || stepping > 0xf) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    x86_cpu_def = 0;
                    goto error;
                }
                x86_cpu_def->stepping = stepping;
            } else {
                fprintf(stderr, "unrecognized feature %s\n", featurestr);
                x86_cpu_def = 0;
                goto error;
            }
        } else {
            fprintf(stderr, "feature string `%s' not in format (+feature|-feature|feature=xyz)\n", featurestr);
            x86_cpu_def = 0;
            goto error;
        }
        featurestr = strtok(NULL, ",");
    }
    x86_cpu_def->features |= plus_features;
    x86_cpu_def->ext_features |= plus_ext_features;
    x86_cpu_def->ext2_features |= plus_ext2_features;
    x86_cpu_def->ext3_features |= plus_ext3_features;
    x86_cpu_def->features &= ~minus_features;
    x86_cpu_def->ext_features &= ~minus_ext_features;
    x86_cpu_def->ext2_features &= ~minus_ext2_features;
    x86_cpu_def->ext3_features &= ~minus_ext3_features;
    free(s);
    return 0;

error:
    free(s);
    return -1;
}

void x86_cpu_list (FILE *f, int (*cpu_fprintf)(FILE *f, const char *fmt, ...))
{
    unsigned int i;

    for (i = 0; i < sizeof(x86_defs) / sizeof(x86_def_t); i++)
        (*cpu_fprintf)(f, "x86 %16s\n", x86_defs[i].name);
}

static int cpu_x86_register (CPUX86State *env, const char *cpu_model)
{
    x86_def_t def1, *def = &def1;

    if (cpu_x86_find_by_name(def, cpu_model) < 0)
        return -1;
    if (def->vendor1) {
        env->cpuid_vendor1 = def->vendor1;
        env->cpuid_vendor2 = def->vendor2;
        env->cpuid_vendor3 = def->vendor3;
    } else {
        env->cpuid_vendor1 = 0x756e6547; /* "Genu" */
        env->cpuid_vendor2 = 0x49656e69; /* "ineI" */
        env->cpuid_vendor3 = 0x6c65746e; /* "ntel" */
    }
    env->cpuid_level = def->level;
    env->cpuid_version = (def->family << 8) | (def->model << 4) | def->stepping;
    env->cpuid_features = def->features;
    env->pat = 0x0007040600070406ULL;
    env->cpuid_ext_features = def->ext_features;
    env->cpuid_ext2_features = def->ext2_features;
    env->cpuid_xlevel = def->xlevel;
    env->cpuid_ext3_features = def->ext3_features;
    {
        const char *model_id = "QEMU Virtual CPU version " QEMU_VERSION;
        int c, len, i;
        len = strlen(model_id);
        for(i = 0; i < 48; i++) {
            if (i >= len)
                c = '\0';
            else
                c = model_id[i];
            env->cpuid_model[i >> 2] |= c << (8 * (i & 3));
        }
    }
    return 0;
}

/* NOTE: must be called outside the CPU execute loop */
void cpu_reset(CPUX86State *env)
{
    int i;

    memset(env, 0, offsetof(CPUX86State, breakpoints));

    tlb_flush(env, 1);

    env->old_exception = -1;

    /* init to reset state */

#ifdef CONFIG_SOFTMMU
    env->hflags |= HF_SOFTMMU_MASK;
#endif
    env->hflags |= HF_GIF_MASK;

    cpu_x86_update_cr0(env, 0x60000010);
    env->a20_mask = 0xffffffff;
    env->smbase = 0x30000;

    env->idt.limit = 0xffff;
    env->gdt.limit = 0xffff;
    env->ldt.limit = 0xffff;
    env->ldt.flags = DESC_P_MASK;
    env->tr.limit = 0xffff;
    env->tr.flags = DESC_P_MASK;

    cpu_x86_load_seg_cache(env, R_CS, 0xf000, 0xffff0000, 0xffff, 0);
    cpu_x86_load_seg_cache(env, R_DS, 0, 0, 0xffff, 0);
    cpu_x86_load_seg_cache(env, R_ES, 0, 0, 0xffff, 0);
    cpu_x86_load_seg_cache(env, R_SS, 0, 0, 0xffff, 0);
    cpu_x86_load_seg_cache(env, R_FS, 0, 0, 0xffff, 0);
    cpu_x86_load_seg_cache(env, R_GS, 0, 0, 0xffff, 0);

    env->eip = 0xfff0;
    env->regs[R_EDX] = env->cpuid_version;

    env->eflags = 0x2;

    /* FPU init */
    for(i = 0;i < 8; i++)
        env->fptags[i] = 1;
    env->fpuc = 0x37f;

    env->mxcsr = 0x1f80;
}

void cpu_x86_close(CPUX86State *env)
{
    free(env);
}

/***********************************************************/
/* x86 debug */

static const char *cc_op_str[] = {
    "DYNAMIC",
    "EFLAGS",

    "MULB",
    "MULW",
    "MULL",
    "MULQ",

    "ADDB",
    "ADDW",
    "ADDL",
    "ADDQ",

    "ADCB",
    "ADCW",
    "ADCL",
    "ADCQ",

    "SUBB",
    "SUBW",
    "SUBL",
    "SUBQ",

    "SBBB",
    "SBBW",
    "SBBL",
    "SBBQ",

    "LOGICB",
    "LOGICW",
    "LOGICL",
    "LOGICQ",

    "INCB",
    "INCW",
    "INCL",
    "INCQ",

    "DECB",
    "DECW",
    "DECL",
    "DECQ",

    "SHLB",
    "SHLW",
    "SHLL",
    "SHLQ",

    "SARB",
    "SARW",
    "SARL",
    "SARQ",
};

void cpu_dump_state(CPUState *env, FILE *f,
                    int (*cpu_fprintf)(FILE *f, const char *fmt, ...),
                    int flags)
{
    int eflags, i, nb;
    char cc_op_name[32];
    static const char *seg_name[6] = { "ES", "CS", "SS", "DS", "FS", "GS" };

    eflags = env->eflags;
#ifdef TARGET_X86_64
    if (env->hflags & HF_CS64_MASK) {
        cpu_fprintf(f,
                    "RAX=%016" PRIx64 " RBX=%016" PRIx64 " RCX=%016" PRIx64 " RDX=%016" PRIx64 "\n"
                    "RSI=%016" PRIx64 " RDI=%016" PRIx64 " RBP=%016" PRIx64 " RSP=%016" PRIx64 "\n"
                    "R8 =%016" PRIx64 " R9 =%016" PRIx64 " R10=%016" PRIx64 " R11=%016" PRIx64 "\n"
                    "R12=%016" PRIx64 " R13=%016" PRIx64 " R14=%016" PRIx64 " R15=%016" PRIx64 "\n"
                    "RIP=%016" PRIx64 " RFL=%08x [%c%c%c%c%c%c%c] CPL=%d II=%d A20=%d SMM=%d HLT=%d\n",
                    env->regs[R_EAX],
                    env->regs[R_EBX],
                    env->regs[R_ECX],
                    env->regs[R_EDX],
                    env->regs[R_ESI],
                    env->regs[R_EDI],
                    env->regs[R_EBP],
                    env->regs[R_ESP],
                    env->regs[8],
                    env->regs[9],
                    env->regs[10],
                    env->regs[11],
                    env->regs[12],
                    env->regs[13],
                    env->regs[14],
                    env->regs[15],
                    env->eip, eflags,
                    eflags & DF_MASK ? 'D' : '-',
                    eflags & CC_O ? 'O' : '-',
                    eflags & CC_S ? 'S' : '-',
                    eflags & CC_Z ? 'Z' : '-',
                    eflags & CC_A ? 'A' : '-',
                    eflags & CC_P ? 'P' : '-',
                    eflags & CC_C ? 'C' : '-',
                    env->hflags & HF_CPL_MASK,
                    (env->hflags >> HF_INHIBIT_IRQ_SHIFT) & 1,
                    (env->a20_mask >> 20) & 1,
                    (env->hflags >> HF_SMM_SHIFT) & 1,
                    (env->hflags >> HF_HALTED_SHIFT) & 1);
    } else
#endif
    {
        cpu_fprintf(f, "EAX=%08x EBX=%08x ECX=%08x EDX=%08x\n"
                    "ESI=%08x EDI=%08x EBP=%08x ESP=%08x\n"
                    "EIP=%08x EFL=%08x [%c%c%c%c%c%c%c] CPL=%d II=%d A20=%d SMM=%d HLT=%d\n",
                    (uint32_t)env->regs[R_EAX],
                    (uint32_t)env->regs[R_EBX],
                    (uint32_t)env->regs[R_ECX],
                    (uint32_t)env->regs[R_EDX],
                    (uint32_t)env->regs[R_ESI],
                    (uint32_t)env->regs[R_EDI],
                    (uint32_t)env->regs[R_EBP],
                    (uint32_t)env->regs[R_ESP],
                    (uint32_t)env->eip, eflags,
                    eflags & DF_MASK ? 'D' : '-',
                    eflags & CC_O ? 'O' : '-',
                    eflags & CC_S ? 'S' : '-',
                    eflags & CC_Z ? 'Z' : '-',
                    eflags & CC_A ? 'A' : '-',
                    eflags & CC_P ? 'P' : '-',
                    eflags & CC_C ? 'C' : '-',
                    env->hflags & HF_CPL_MASK,
                    (env->hflags >> HF_INHIBIT_IRQ_SHIFT) & 1,
                    (env->a20_mask >> 20) & 1,
                    (env->hflags >> HF_SMM_SHIFT) & 1,
                    (env->hflags >> HF_HALTED_SHIFT) & 1);
    }

#ifdef TARGET_X86_64
    if (env->hflags & HF_LMA_MASK) {
        for(i = 0; i < 6; i++) {
            SegmentCache *sc = &env->segs[i];
            cpu_fprintf(f, "%s =%04x %016" PRIx64 " %08x %08x\n",
                        seg_name[i],
                        sc->selector,
                        sc->base,
                        sc->limit,
                        sc->flags);
        }
        cpu_fprintf(f, "LDT=%04x %016" PRIx64 " %08x %08x\n",
                    env->ldt.selector,
                    env->ldt.base,
                    env->ldt.limit,
                    env->ldt.flags);
        cpu_fprintf(f, "TR =%04x %016" PRIx64 " %08x %08x\n",
                    env->tr.selector,
                    env->tr.base,
                    env->tr.limit,
                    env->tr.flags);
        cpu_fprintf(f, "GDT=     %016" PRIx64 " %08x\n",
                    env->gdt.base, env->gdt.limit);
        cpu_fprintf(f, "IDT=     %016" PRIx64 " %08x\n",
                    env->idt.base, env->idt.limit);
        cpu_fprintf(f, "CR0=%08x CR2=%016" PRIx64 " CR3=%016" PRIx64 " CR4=%08x\n",
                    (uint32_t)env->cr[0],
                    env->cr[2],
                    env->cr[3],
                    (uint32_t)env->cr[4]);
    } else
#endif
    {
        for(i = 0; i < 6; i++) {
            SegmentCache *sc = &env->segs[i];
            cpu_fprintf(f, "%s =%04x %08x %08x %08x\n",
                        seg_name[i],
                        sc->selector,
                        (uint32_t)sc->base,
                        sc->limit,
                        sc->flags);
        }
        cpu_fprintf(f, "LDT=%04x %08x %08x %08x\n",
                    env->ldt.selector,
                    (uint32_t)env->ldt.base,
                    env->ldt.limit,
                    env->ldt.flags);
        cpu_fprintf(f, "TR =%04x %08x %08x %08x\n",
                    env->tr.selector,
                    (uint32_t)env->tr.base,
                    env->tr.limit,
                    env->tr.flags);
        cpu_fprintf(f, "GDT=     %08x %08x\n",
                    (uint32_t)env->gdt.base, env->gdt.limit);
        cpu_fprintf(f, "IDT=     %08x %08x\n",
                    (uint32_t)env->idt.base, env->idt.limit);
        cpu_fprintf(f, "CR0=%08x CR2=%08x CR3=%08x CR4=%08x\n",
                    (uint32_t)env->cr[0],
                    (uint32_t)env->cr[2],
                    (uint32_t)env->cr[3],
                    (uint32_t)env->cr[4]);
    }
    if (flags & X86_DUMP_CCOP) {
        if ((unsigned)env->cc_op < CC_OP_NB)
            snprintf(cc_op_name, sizeof(cc_op_name), "%s", cc_op_str[env->cc_op]);
        else
            snprintf(cc_op_name, sizeof(cc_op_name), "[%d]", env->cc_op);
#ifdef TARGET_X86_64
        if (env->hflags & HF_CS64_MASK) {
            cpu_fprintf(f, "CCS=%016" PRIx64 " CCD=%016" PRIx64 " CCO=%-8s\n",
                        env->cc_src, env->cc_dst,
                        cc_op_name);
        } else
#endif
        {
            cpu_fprintf(f, "CCS=%08x CCD=%08x CCO=%-8s\n",
                        (uint32_t)env->cc_src, (uint32_t)env->cc_dst,
                        cc_op_name);
        }
    }
    if (flags & X86_DUMP_FPU) {
        int fptag;
        fptag = 0;
        for(i = 0; i < 8; i++) {
            fptag |= ((!env->fptags[i]) << i);
        }
        cpu_fprintf(f, "FCW=%04x FSW=%04x [ST=%d] FTW=%02x MXCSR=%08x\n",
                    env->fpuc,
                    (env->fpus & ~0x3800) | (env->fpstt & 0x7) << 11,
                    env->fpstt,
                    fptag,
                    env->mxcsr);
        for(i=0;i<8;i++) {
#if defined(USE_X86LDOUBLE)
            union {
                long double d;
                struct {
                    uint64_t lower;
                    uint16_t upper;
                } l;
            } tmp;
            tmp.d = env->fpregs[i].d;
            cpu_fprintf(f, "FPR%d=%016" PRIx64 " %04x",
                        i, tmp.l.lower, tmp.l.upper);
#else
            cpu_fprintf(f, "FPR%d=%016" PRIx64,
                        i, env->fpregs[i].mmx.q);
#endif
            if ((i & 1) == 1)
                cpu_fprintf(f, "\n");
            else
                cpu_fprintf(f, " ");
        }
        if (env->hflags & HF_CS64_MASK)
            nb = 16;
        else
            nb = 8;
        for(i=0;i<nb;i++) {
            cpu_fprintf(f, "XMM%02d=%08x%08x%08x%08x",
                        i,
                        env->xmm_regs[i].XMM_L(3),
                        env->xmm_regs[i].XMM_L(2),
                        env->xmm_regs[i].XMM_L(1),
                        env->xmm_regs[i].XMM_L(0));
            if ((i & 1) == 1)
                cpu_fprintf(f, "\n");
            else
                cpu_fprintf(f, " ");
        }
    }
}

/***********************************************************/
/* x86 mmu */
/* XXX: add PGE support */

void cpu_x86_set_a20(CPUX86State *env, int a20_state)
{
    a20_state = (a20_state != 0);
    if (a20_state != ((env->a20_mask >> 20) & 1)) {
#if defined(DEBUG_MMU)
        printf("A20 update: a20=%d\n", a20_state);
#endif
        /* if the cpu is currently executing code, we must unlink it and
           all the potentially executing TB */
        cpu_interrupt(env, CPU_INTERRUPT_EXITTB);

        /* when a20 is changed, all the MMU mappings are invalid, so
           we must flush everything */
        tlb_flush(env, 1);
        env->a20_mask = 0xffefffff | (a20_state << 20);
    }
}

void cpu_x86_update_cr0(CPUX86State *env, uint32_t new_cr0)
{
    int pe_state;

#if defined(DEBUG_MMU)
    printf("CR0 update: CR0=0x%08x\n", new_cr0);
#endif
    if ((new_cr0 & (CR0_PG_MASK | CR0_WP_MASK | CR0_PE_MASK)) !=
        (env->cr[0] & (CR0_PG_MASK | CR0_WP_MASK | CR0_PE_MASK))) {
        tlb_flush(env, 1);
    }

#ifdef TARGET_X86_64
    if (!(env->cr[0] & CR0_PG_MASK) && (new_cr0 & CR0_PG_MASK) &&
        (env->efer & MSR_EFER_LME)) {
        /* enter in long mode */
        /* XXX: generate an exception */
        if (!(env->cr[4] & CR4_PAE_MASK))
            return;
        env->efer |= MSR_EFER_LMA;
        env->hflags |= HF_LMA_MASK;
    } else if ((env->cr[0] & CR0_PG_MASK) && !(new_cr0 & CR0_PG_MASK) &&
               (env->efer & MSR_EFER_LMA)) {
        /* exit long mode */
        env->efer &= ~MSR_EFER_LMA;
        env->hflags &= ~(HF_LMA_MASK | HF_CS64_MASK);
        env->eip &= 0xffffffff;
    }
#endif
    env->cr[0] = new_cr0 | CR0_ET_MASK;

    /* update PE flag in hidden flags */
    pe_state = (env->cr[0] & CR0_PE_MASK);
    env->hflags = (env->hflags & ~HF_PE_MASK) | (pe_state << HF_PE_SHIFT);
    /* ensure that ADDSEG is always set in real mode */
    env->hflags |= ((pe_state ^ 1) << HF_ADDSEG_SHIFT);
    /* update FPU flags */
    env->hflags = (env->hflags & ~(HF_MP_MASK | HF_EM_MASK | HF_TS_MASK)) |
        ((new_cr0 << (HF_MP_SHIFT - 1)) & (HF_MP_MASK | HF_EM_MASK | HF_TS_MASK));
}

/* XXX: in legacy PAE mode, generate a GPF if reserved bits are set in
   the PDPT */
void cpu_x86_update_cr3(CPUX86State *env, target_ulong new_cr3)
{
    env->cr[3] = new_cr3;
    if (env->cr[0] & CR0_PG_MASK) {
#if defined(DEBUG_MMU)
        printf("CR3 update: CR3=" TARGET_FMT_lx "\n", new_cr3);
#endif
        tlb_flush(env, 0);
    }
}

void cpu_x86_update_cr4(CPUX86State *env, uint32_t new_cr4)
{
#if defined(DEBUG_MMU)
    printf("CR4 update: CR4=%08x\n", (uint32_t)env->cr[4]);
#endif
    if ((new_cr4 & (CR4_PGE_MASK | CR4_PAE_MASK | CR4_PSE_MASK)) !=
        (env->cr[4] & (CR4_PGE_MASK | CR4_PAE_MASK | CR4_PSE_MASK))) {
        tlb_flush(env, 1);
    }
    /* SSE handling */
    if (!(env->cpuid_features & CPUID_SSE))
        new_cr4 &= ~CR4_OSFXSR_MASK;
    if (new_cr4 & CR4_OSFXSR_MASK)
        env->hflags |= HF_OSFXSR_MASK;
    else
        env->hflags &= ~HF_OSFXSR_MASK;

    env->cr[4] = new_cr4;
}

/* XXX: also flush 4MB pages */
void cpu_x86_flush_tlb(CPUX86State *env, target_ulong addr)
{
    tlb_flush_page(env, addr);
}

#if defined(CONFIG_USER_ONLY)

int cpu_x86_handle_mmu_fault(CPUX86State *env, target_ulong addr,
                             int is_write, int mmu_idx, int is_softmmu)
{
    /* user mode only emulation */
    is_write &= 1;
    env->cr[2] = addr;
    env->error_code = (is_write << PG_ERROR_W_BIT);
    env->error_code |= PG_ERROR_U_MASK;
    env->exception_index = EXCP0E_PAGE;
    return 1;
}

target_phys_addr_t cpu_get_phys_page_debug(CPUState *env, target_ulong addr)
{
    return addr;
}

#else

#define PHYS_ADDR_MASK 0xfffff000

/* return value:
   -1 = cannot handle fault
   0  = nothing more to do
   1  = generate PF fault
   2  = soft MMU activation required for this block
*/
int cpu_x86_handle_mmu_fault(CPUX86State *env, target_ulong addr,
                             int is_write1, int mmu_idx, int is_softmmu)
{
    uint64_t ptep, pte;
    uint32_t pdpe_addr, pde_addr, pte_addr;
    int error_code, is_dirty, prot, page_size, ret, is_write, is_user;
    unsigned long paddr, page_offset;
    target_ulong vaddr, virt_addr;

    is_user = mmu_idx == MMU_USER_IDX;
#if defined(DEBUG_MMU)
    printf("MMU fault: addr=" TARGET_FMT_lx " w=%d u=%d eip=" TARGET_FMT_lx "\n",
           addr, is_write1, is_user, env->eip);
#endif
    is_write = is_write1 & 1;

    if (!(env->cr[0] & CR0_PG_MASK)) {
        pte = addr;
        virt_addr = addr & TARGET_PAGE_MASK;
        prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;
        page_size = 4096;
        goto do_mapping;
    }

    if (env->cr[4] & CR4_PAE_MASK) {
        uint64_t pde, pdpe;

        /* XXX: we only use 32 bit physical addresses */
#ifdef TARGET_X86_64
        if (env->hflags & HF_LMA_MASK) {
            uint32_t pml4e_addr;
            uint64_t pml4e;
            int32_t sext;

            /* test virtual address sign extension */
            sext = (int64_t)addr >> 47;
            if (sext != 0 && sext != -1) {
                env->error_code = 0;
                env->exception_index = EXCP0D_GPF;
                return 1;
            }

            pml4e_addr = ((env->cr[3] & ~0xfff) + (((addr >> 39) & 0x1ff) << 3)) &
                env->a20_mask;
            pml4e = ldq_phys(pml4e_addr);
            if (!(pml4e & PG_PRESENT_MASK)) {
                error_code = 0;
                goto do_fault;
            }
            if (!(env->efer & MSR_EFER_NXE) && (pml4e & PG_NX_MASK)) {
                error_code = PG_ERROR_RSVD_MASK;
                goto do_fault;
            }
            if (!(pml4e & PG_ACCESSED_MASK)) {
                pml4e |= PG_ACCESSED_MASK;
                stl_phys_notdirty(pml4e_addr, pml4e);
            }
            ptep = pml4e ^ PG_NX_MASK;
            pdpe_addr = ((pml4e & PHYS_ADDR_MASK) + (((addr >> 30) & 0x1ff) << 3)) &
                env->a20_mask;
            pdpe = ldq_phys(pdpe_addr);
            if (!(pdpe & PG_PRESENT_MASK)) {
                error_code = 0;
                goto do_fault;
            }
            if (!(env->efer & MSR_EFER_NXE) && (pdpe & PG_NX_MASK)) {
                error_code = PG_ERROR_RSVD_MASK;
                goto do_fault;
            }
            ptep &= pdpe ^ PG_NX_MASK;
            if (!(pdpe & PG_ACCESSED_MASK)) {
                pdpe |= PG_ACCESSED_MASK;
                stl_phys_notdirty(pdpe_addr, pdpe);
            }
        } else
#endif
        {
            /* XXX: load them when cr3 is loaded ? */
            pdpe_addr = ((env->cr[3] & ~0x1f) + ((addr >> 27) & 0x18)) &
                env->a20_mask;
            pdpe = ldq_phys(pdpe_addr);
            if (!(pdpe & PG_PRESENT_MASK)) {
                error_code = 0;
                goto do_fault;
            }
            ptep = PG_NX_MASK | PG_USER_MASK | PG_RW_MASK;
        }

        pde_addr = ((pdpe & PHYS_ADDR_MASK) + (((addr >> 21) & 0x1ff) << 3)) &
            env->a20_mask;
        pde = ldq_phys(pde_addr);
        if (!(pde & PG_PRESENT_MASK)) {
            error_code = 0;
            goto do_fault;
        }
        if (!(env->efer & MSR_EFER_NXE) && (pde & PG_NX_MASK)) {
            error_code = PG_ERROR_RSVD_MASK;
            goto do_fault;
        }
        ptep &= pde ^ PG_NX_MASK;
        if (pde & PG_PSE_MASK) {
            /* 2 MB page */
            page_size = 2048 * 1024;
            ptep ^= PG_NX_MASK;
            if ((ptep & PG_NX_MASK) && is_write1 == 2)
                goto do_fault_protect;
            if (is_user) {
                if (!(ptep & PG_USER_MASK))
                    goto do_fault_protect;
                if (is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            } else {
                if ((env->cr[0] & CR0_WP_MASK) &&
                    is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            }
            is_dirty = is_write && !(pde & PG_DIRTY_MASK);
            if (!(pde & PG_ACCESSED_MASK) || is_dirty) {
                pde |= PG_ACCESSED_MASK;
                if (is_dirty)
                    pde |= PG_DIRTY_MASK;
                stl_phys_notdirty(pde_addr, pde);
            }
            /* align to page_size */
            pte = pde & ((PHYS_ADDR_MASK & ~(page_size - 1)) | 0xfff);
            virt_addr = addr & ~(page_size - 1);
        } else {
            /* 4 KB page */
            if (!(pde & PG_ACCESSED_MASK)) {
                pde |= PG_ACCESSED_MASK;
                stl_phys_notdirty(pde_addr, pde);
            }
            pte_addr = ((pde & PHYS_ADDR_MASK) + (((addr >> 12) & 0x1ff) << 3)) &
                env->a20_mask;
            pte = ldq_phys(pte_addr);
            if (!(pte & PG_PRESENT_MASK)) {
                error_code = 0;
                goto do_fault;
            }
            if (!(env->efer & MSR_EFER_NXE) && (pte & PG_NX_MASK)) {
                error_code = PG_ERROR_RSVD_MASK;
                goto do_fault;
            }
            /* combine pde and pte nx, user and rw protections */
            ptep &= pte ^ PG_NX_MASK;
            ptep ^= PG_NX_MASK;
            if ((ptep & PG_NX_MASK) && is_write1 == 2)
                goto do_fault_protect;
            if (is_user) {
                if (!(ptep & PG_USER_MASK))
                    goto do_fault_protect;
                if (is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            } else {
                if ((env->cr[0] & CR0_WP_MASK) &&
                    is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            }
            is_dirty = is_write && !(pte & PG_DIRTY_MASK);
            if (!(pte & PG_ACCESSED_MASK) || is_dirty) {
                pte |= PG_ACCESSED_MASK;
                if (is_dirty)
                    pte |= PG_DIRTY_MASK;
                stl_phys_notdirty(pte_addr, pte);
            }
            page_size = 4096;
            virt_addr = addr & ~0xfff;
            pte = pte & (PHYS_ADDR_MASK | 0xfff);
        }
    } else {
        uint32_t pde;

        /* page directory entry */
        pde_addr = ((env->cr[3] & ~0xfff) + ((addr >> 20) & 0xffc)) &
            env->a20_mask;
        pde = ldl_phys(pde_addr);
        if (!(pde & PG_PRESENT_MASK)) {
            error_code = 0;
            goto do_fault;
        }
        /* if PSE bit is set, then we use a 4MB page */
        if ((pde & PG_PSE_MASK) && (env->cr[4] & CR4_PSE_MASK)) {
            page_size = 4096 * 1024;
            if (is_user) {
                if (!(pde & PG_USER_MASK))
                    goto do_fault_protect;
                if (is_write && !(pde & PG_RW_MASK))
                    goto do_fault_protect;
            } else {
                if ((env->cr[0] & CR0_WP_MASK) &&
                    is_write && !(pde & PG_RW_MASK))
                    goto do_fault_protect;
            }
            is_dirty = is_write && !(pde & PG_DIRTY_MASK);
            if (!(pde & PG_ACCESSED_MASK) || is_dirty) {
                pde |= PG_ACCESSED_MASK;
                if (is_dirty)
                    pde |= PG_DIRTY_MASK;
                stl_phys_notdirty(pde_addr, pde);
            }

            pte = pde & ~( (page_size - 1) & ~0xfff); /* align to page_size */
            ptep = pte;
            virt_addr = addr & ~(page_size - 1);
        } else {
            if (!(pde & PG_ACCESSED_MASK)) {
                pde |= PG_ACCESSED_MASK;
                stl_phys_notdirty(pde_addr, pde);
            }

            /* page directory entry */
            pte_addr = ((pde & ~0xfff) + ((addr >> 10) & 0xffc)) &
                env->a20_mask;
            pte = ldl_phys(pte_addr);
            if (!(pte & PG_PRESENT_MASK)) {
                error_code = 0;
                goto do_fault;
            }
            /* combine pde and pte user and rw protections */
            ptep = pte & pde;
            if (is_user) {
                if (!(ptep & PG_USER_MASK))
                    goto do_fault_protect;
                if (is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            } else {
                if ((env->cr[0] & CR0_WP_MASK) &&
                    is_write && !(ptep & PG_RW_MASK))
                    goto do_fault_protect;
            }
            is_dirty = is_write && !(pte & PG_DIRTY_MASK);
            if (!(pte & PG_ACCESSED_MASK) || is_dirty) {
                pte |= PG_ACCESSED_MASK;
                if (is_dirty)
                    pte |= PG_DIRTY_MASK;
                stl_phys_notdirty(pte_addr, pte);
            }
            page_size = 4096;
            virt_addr = addr & ~0xfff;
        }
    }
    /* the page can be put in the TLB */
    prot = PAGE_READ;
    if (!(ptep & PG_NX_MASK))
        prot |= PAGE_EXEC;
    if (pte & PG_DIRTY_MASK) {
        /* only set write access if already dirty... otherwise wait
           for dirty access */
        if (is_user) {
            if (ptep & PG_RW_MASK)
                prot |= PAGE_WRITE;
        } else {
            if (!(env->cr[0] & CR0_WP_MASK) ||
                (ptep & PG_RW_MASK))
                prot |= PAGE_WRITE;
        }
    }
 do_mapping:
    pte = pte & env->a20_mask;

    /* Even if 4MB pages, we map only one 4KB page in the cache to
       avoid filling it too fast */
    page_offset = (addr & TARGET_PAGE_MASK) & (page_size - 1);
    paddr = (pte & TARGET_PAGE_MASK) + page_offset;
    vaddr = virt_addr + page_offset;

    ret = tlb_set_page_exec(env, vaddr, paddr, prot, mmu_idx, is_softmmu);
    return ret;
 do_fault_protect:
    error_code = PG_ERROR_P_MASK;
 do_fault:
    error_code |= (is_write << PG_ERROR_W_BIT);
    if (is_user)
        error_code |= PG_ERROR_U_MASK;
    if (is_write1 == 2 &&
        (env->efer & MSR_EFER_NXE) &&
        (env->cr[4] & CR4_PAE_MASK))
        error_code |= PG_ERROR_I_D_MASK;
    if (INTERCEPTEDl(_exceptions, 1 << EXCP0E_PAGE)) {
        stq_phys(env->vm_vmcb + offsetof(struct vmcb, control.exit_info_2), addr);
    } else {
        env->cr[2] = addr;
    }
    env->error_code = error_code;
    env->exception_index = EXCP0E_PAGE;
    /* the VMM will handle this */
    if (INTERCEPTEDl(_exceptions, 1 << EXCP0E_PAGE))
        return 2;
    return 1;
}

target_phys_addr_t cpu_get_phys_page_debug(CPUState *env, target_ulong addr)
{
    uint32_t pde_addr, pte_addr;
    uint32_t pde, pte, paddr, page_offset, page_size;

    if (env->cr[4] & CR4_PAE_MASK) {
        uint32_t pdpe_addr, pde_addr, pte_addr;
        uint32_t pdpe;

        /* XXX: we only use 32 bit physical addresses */
#ifdef TARGET_X86_64
        if (env->hflags & HF_LMA_MASK) {
            uint32_t pml4e_addr, pml4e;
            int32_t sext;

            /* test virtual address sign extension */
            sext = (int64_t)addr >> 47;
            if (sext != 0 && sext != -1)
                return -1;

            pml4e_addr = ((env->cr[3] & ~0xfff) + (((addr >> 39) & 0x1ff) << 3)) &
                env->a20_mask;
            pml4e = ldl_phys(pml4e_addr);
            if (!(pml4e & PG_PRESENT_MASK))
                return -1;

            pdpe_addr = ((pml4e & ~0xfff) + (((addr >> 30) & 0x1ff) << 3)) &
                env->a20_mask;
            pdpe = ldl_phys(pdpe_addr);
            if (!(pdpe & PG_PRESENT_MASK))
                return -1;
        } else
#endif
        {
            pdpe_addr = ((env->cr[3] & ~0x1f) + ((addr >> 27) & 0x18)) &
                env->a20_mask;
            pdpe = ldl_phys(pdpe_addr);
            if (!(pdpe & PG_PRESENT_MASK))
                return -1;
        }

        pde_addr = ((pdpe & ~0xfff) + (((addr >> 21) & 0x1ff) << 3)) &
            env->a20_mask;
        pde = ldl_phys(pde_addr);
        if (!(pde & PG_PRESENT_MASK)) {
            return -1;
        }
        if (pde & PG_PSE_MASK) {
            /* 2 MB page */
            page_size = 2048 * 1024;
            pte = pde & ~( (page_size - 1) & ~0xfff); /* align to page_size */
        } else {
            /* 4 KB page */
            pte_addr = ((pde & ~0xfff) + (((addr >> 12) & 0x1ff) << 3)) &
                env->a20_mask;
            page_size = 4096;
            pte = ldl_phys(pte_addr);
        }
    } else {
        if (!(env->cr[0] & CR0_PG_MASK)) {
            pte = addr;
            page_size = 4096;
        } else {
            /* page directory entry */
            pde_addr = ((env->cr[3] & ~0xfff) + ((addr >> 20) & 0xffc)) & env->a20_mask;
            pde = ldl_phys(pde_addr);
            if (!(pde & PG_PRESENT_MASK))
                return -1;
            if ((pde & PG_PSE_MASK) && (env->cr[4] & CR4_PSE_MASK)) {
                pte = pde & ~0x003ff000; /* align to 4MB */
                page_size = 4096 * 1024;
            } else {
                /* page directory entry */
                pte_addr = ((pde & ~0xfff) + ((addr >> 10) & 0xffc)) & env->a20_mask;
                pte = ldl_phys(pte_addr);
                if (!(pte & PG_PRESENT_MASK))
                    return -1;
                page_size = 4096;
            }
        }
        pte = pte & env->a20_mask;
    }

    page_offset = (addr & TARGET_PAGE_MASK) & (page_size - 1);
    paddr = (pte & TARGET_PAGE_MASK) + page_offset;
    return paddr;
}
#endif /* !CONFIG_USER_ONLY */
