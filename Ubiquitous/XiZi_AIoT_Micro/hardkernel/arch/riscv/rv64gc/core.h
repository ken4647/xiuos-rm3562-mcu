/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
 * @file core.h
 * @brief cortex-a55 core function
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.04.11
 */

/*************************************************
File name: core.h
Description: cortex-a55 core function
Others:
History:
Author: AIIT XUOS Lab
Modification:
*************************************************/

#pragma once

// Interrupt control bits
#define NO_INT 0x80 // disable IRQ.
#define DIS_INT 0xc0 // disable both IRQ and FIQ.

#define MODE_STACK_SIZE 0x1000

//! @name SPSR fields
//@{
#define SPSR_EL1_N (1 << 31) //!< Negative
#define SPSR_EL1_Z (1 << 30) //!< Zero
#define SPSR_EL1_C (1 << 29) //!< Carry
#define SPSR_EL1_V (1 << 28) //!< Overflow
#define SPSR_EL1_SS (1 << 21) //!< Software Step
#define SPSR_EL1_IL (1 << 20) //!< Illegal Exception
#define SPSR_EL1_D (1 << 9) //!< Debug mask
#define SPSR_EL1_A (1 << 8) //!< SError mask
#define SPSR_EL1_I (1 << 7) //!< IRQ mask
#define SPSR_EL1_F (1 << 6) //!< FIQ mask
#define SPSR_EL1_M (1 << 4) //!< Execution state    0=64-bit  1=32-bit
#define SPSR_EL1_MODE (0x7) //!< Current processor mode
//@}

//! @name Interrupt enable bits in SPSR
//@{
#define I_BIT 0x80 //!< When I bit is set, IRQ is disabled
#define F_BIT 0x40 //!< When F bit is set, FIQ is disabled
//@}

// ARM Modes         t indicates selecting  sp_el0 pointer, h indicates selecting  sp_eln pointer
#define SPSR_MODE_MASK 0x0f
#define ARM_MODE_EL0_t 0x00
#define ARM_MODE_EL1_t 0x04
#define ARM_MODE_EL1_h 0x05
#define ARM_MODE_EL2_t 0x08
#define ARM_MODE_EL2_h 0x09
#define ARM_MODE_EL3_t 0x0c
#define ARM_MODE_EL3_h 0x0d

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <string.h>

#include "cortex.h"
#include "asm/csr.h"


#define NR_CPU 1 // maximum number of CPUs


struct __riscv_d_ext_state {
    uint64_t f[32];
    uint32_t fcsr;
};
/* Refer to struct thread_struct in Linux */
/* CPU-specific state of a task */
struct context {
    /* Callee-saved registers */
    unsigned long ra;
    unsigned long sp;	/* Kernel mode stack */
    unsigned long s[12];	/* s[0]: frame pointer */
    struct __riscv_d_ext_state fstate;
    unsigned long bad_cause;
};

/// @brief init task context, set return address to trap return
/// @param  ctx
extern void task_prepare_enter(void);
__attribute__((__always_inline__)) static inline void arch_init_context(struct context* ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->ra = (uintptr_t)(task_prepare_enter);
}

__attribute__((__always_inline__)) static inline void arch_context_set_sp(struct context* ctx, unsigned long sp)
{
    ctx->sp = sp;
}

/* Refer to struct pt_regs in Linux */
struct trapframe {
    unsigned long epc;
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    /* Supervisor/Machine CSRs */
    unsigned long status;
    unsigned long badaddr;
    unsigned long cause;
    /* a0 value before the syscall */
    unsigned long orig_a0;
};

/// @brief init task trapframe
/// @param tf
/// @param sp
/// @param pc
__attribute__((__always_inline__)) static inline void arch_init_trapframe(struct trapframe* tf, uintptr_t sp, uintptr_t pc)
{
    memset(tf, 0, sizeof(*tf));
    tf->sp = sp;
    tf->epc = pc;
    tf->status = SR_PIE;
}

/// @brief set pc and sp to trapframe
/// @param tf
/// @param sp
/// @param pc
__attribute__((__always_inline__)) static inline void arch_trapframe_set_sp_pc(struct trapframe* tf, uintptr_t sp, uintptr_t pc)
{
    tf->sp = sp;
    tf->epc = pc;
}

/// @brief set params of main(int argc, char** argv) to trapframe (argc, argv)
/// @param tf
/// @param argc
/// @param argv
__attribute__((__always_inline__)) static inline void arch_set_main_params(struct trapframe* tf, int argc, uintptr_t argv)
{
    tf->a0 = (uint64_t)argc;
    tf->a1 = (uint64_t)argv;
}

/// @brief retrieve params to trapframe (up to max number of 6) and pass it to syscall()
/// @param sys_num
/// @param param1
/// @param param2
/// @param param3
/// @param param4
/// @param param5
/// @return
extern int syscall(int sys_num, uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4);
__attribute__((__always_inline__)) static inline int arch_syscall(struct trapframe* tf, int* syscall_num)
{
    // call syscall
    *syscall_num = tf->a7;
    return syscall(*syscall_num, tf->a0, tf->a1, tf->a2, tf->a3);
}

/// @brief set return reg to trapframe
/// @param tf
/// @param ret
__attribute__((__always_inline__)) static inline void arch_set_return(struct trapframe* tf, int ret)
{
    tf->a0 = (uint64_t)ret;
}

// TODO: refer to jh7110 Linux
struct thread_info {
	unsigned long		flags;		/* low level flags */
	long			preempt_count;  /* 0=>preemptible, <0=>BUG */
	long			kernel_sp;	/* Kernel stack pointer */
	long			user_sp;	/* User stack pointer */
	long			cpu;
};


void cpu_start_secondary(uint8_t cpu_id);
void start_smp_cache_broadcast(int cpu_id);
#endif
