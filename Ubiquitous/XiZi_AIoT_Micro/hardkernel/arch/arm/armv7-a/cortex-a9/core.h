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
 * @brief cortex-a9 core function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.20
 */

/*************************************************
File name: core.h
Description: cortex-a9 core function
Others:
History:
1. Date: 2023-11-20
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

#pragma once

// Interrupt control bits
#define NO_INT 0xc0 // disable IRQ.
#define DIS_INT 0x80 // disable both IRQ and FIQ.

//! @name CPSR fields
//@{
#define CPSR_N (1 << 31) //!< Negative
#define CPSR_Z (1 << 30) //!< Zero
#define CPSR_C (1 << 29) //!< Carry
#define CPSR_V (1 << 28) //!< Overflow
#define CPSR_Q (1 << 27) //!< Saturation
#define CPSR_E (1 << 9) //!< Endianness
#define CPSR_A (1 << 8) //!< Async abort mask
#define CPSR_I (1 << 7) //!< IRQ mask
#define CPSR_F (1 << 6) //!< FIQ mask
#define CPSR_T (1 << 5) //!< Thumb mode
#define CPSR_MODE (0x1f) //!< Current processor mode
//@}

#define MODE_STACK_SIZE 0x1000

//! @name Interrupt enable bits in CPSR
//@{
#define I_BIT 0x80 //!< When I bit is set, IRQ is disabled
#define F_BIT 0x40 //!< When F bit is set, FIQ is disabled
//@}

// ARM modes.
#define ARM_CPSR_MODE_MASK 0x1f
#define ARM_MODE_USR 0x10
#define ARM_MODE_FIQ 0x11
#define ARM_MODE_IRQ 0x12
#define ARM_MODE_SVC 0x13
#define ARM_MODE_ABT 0x17
#define ARM_MODE_UND 0x1b
#define ARM_MODE_SYS 0x1f

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <string.h>

#include "cortex_a9.h"

#define NR_CPU 4

static inline uint64_t arch_current_tick()
{
    uint32_t tick = 0;
    __asm__ __volatile__("MRC p15, 0, %0, c9, c13, 0" : "=r"(tick)); // %0 应该是输出操作数
    return (uint64_t)tick;
}

__attribute__((always_inline, optimize("O0"))) static inline uint32_t user_mode()
{
    uint32_t val;

    __asm__ __volatile__(
        "mrs %0, cpsr"
        : "=r"(val)
        :
        :);
    val &= ~DIS_INT;
    val &= ~ARM_CPSR_MODE_MASK;
    val |= ARM_MODE_USR;

    return val;
}

__attribute__((always_inline, optimize("O0"))) static inline void cpu_into_low_power()
{
    WFE();
}

__attribute__((always_inline, optimize("O0"))) static inline void cpu_leave_low_power()
{
    SEV();
}

struct context {
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;
} __attribute__((packed));

/// @brief init task context, set return address to trap return
/// @param
extern void task_prepare_enter();
__attribute__((always_inline, optimize("O0"))) static inline void arch_init_context(struct context* ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->lr = (uint32_t)(task_prepare_enter + 4);
}

struct trapframe {
    uint32_t sp_usr;
    uint32_t lr_usr;
    uint32_t lr_svc;
    uint32_t spsr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t pc;
} __attribute__((packed));

/// @brief init task trapframe (*especially the user mode cpsr)
/// @param tf
/// @param sp
/// @param pc
__attribute__((always_inline, optimize("O0"))) static inline void arch_init_trapframe(struct trapframe* tf, uintptr_t sp, uintptr_t pc)
{
    memset(tf, 0, sizeof(*tf));
    tf->spsr = user_mode();
    tf->sp_usr = sp;
    tf->lr_svc = 0;
    tf->lr_usr = 0;
    tf->pc = pc;
}

/// @brief set pc and sp to trapframe
/// @param tf
/// @param sp
/// @param pc
__attribute__((always_inline, optimize("O0"))) static inline void arch_trapframe_set_sp_pc(struct trapframe* tf, uintptr_t sp, uintptr_t pc)
{
    tf->sp_usr = sp;
    tf->pc = pc;
}

/// @brief set params of main(int argc, char** argv) to trapframe (argc, argv)
/// @param tf
/// @param argc
/// @param argv
__attribute__((always_inline, optimize("O0"))) static inline void arch_set_main_params(struct trapframe* tf, int argc, uintptr_t argv)
{
    tf->r0 = (uint32_t)argc;
    tf->r1 = (uint32_t)argv;
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
__attribute__((always_inline, optimize("O0"))) static inline int arch_syscall(struct trapframe* tf, int* syscall_num)
{
    // call syscall
    *syscall_num = tf->r0;
    return syscall(*syscall_num, tf->r1, tf->r2, tf->r3, tf->r4);
}

/// @brief set return reg to trapframe
/// @param tf
/// @param ret
__attribute__((always_inline, optimize("O0"))) static inline void arch_set_return(struct trapframe* tf, int ret)
{
    tf->r0 = (uint32_t)ret;
}

void cpu_start_secondary(uint8_t cpu_id);
void start_smp_cache_broadcast(int cpu_id);
#endif