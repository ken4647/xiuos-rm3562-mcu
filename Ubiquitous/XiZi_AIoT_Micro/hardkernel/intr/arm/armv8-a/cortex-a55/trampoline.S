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
 * @file trampoline.S
 * @brief trap in and out code
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-04-22
 */

/*************************************************
File name: trampoline.S
Description: trap in and out code
Others: 
History:
1. Date: 2024-04-22
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

#include "memlayout.h"

#include "core.h"

.macro savereg
        // make room to save registers.
        sub sp, sp, #272

        // save the registers.
        stp x0, x1, [sp, #16 * 0]
        stp x2, x3, [sp, #16 * 1]
        stp x4, x5, [sp, #16 * 2]
        stp x6, x7, [sp, #16 * 3]
        stp x8, x9, [sp, #16 * 4]
        stp x10, x11, [sp, #16 * 5]
        stp x12, x13, [sp, #16 * 6]
        stp x14, x15, [sp, #16 * 7]
        stp x16, x17, [sp, #16 * 8]
        stp x18, x19, [sp, #16 * 9]
        stp x20, x21, [sp, #16 * 10]
        stp x22, x23, [sp, #16 * 11]
        stp x24, x25, [sp, #16 * 12]
        stp x26, x27, [sp, #16 * 13]
        stp x28, x29, [sp, #16 * 14]
        mrs x9, elr_el1
        mrs x10, spsr_el1
        add x11, sp, #272
        stp x30, x9, [sp, #16 * 15]
        stp x10, x11, [sp, #16 * 16]
.endm

.macro restorereg
        ldp x30, x9, [sp, #16 * 15]
        ldp x10, x11, [sp, #16 * 16]

        msr elr_el1, x9
        msr spsr_el1, x10

        ldp x0, x1, [sp, #16 * 0]
        ldp x2, x3, [sp, #16 * 1]
        ldp x4, x5, [sp, #16 * 2]
        ldp x6, x7, [sp, #16 * 3]
        ldp x8, x9, [sp, #16 * 4]
        ldp x10, x11, [sp, #16 * 5]
        ldp x12, x13, [sp, #16 * 6]
        ldp x14, x15, [sp, #16 * 7]
        ldp x16, x17, [sp, #16 * 8]
        ldp x18, x19, [sp, #16 * 9]
        ldp x20, x21, [sp, #16 * 10]
        ldp x22, x23, [sp, #16 * 11]
        ldp x24, x25, [sp, #16 * 12]
        ldp x26, x27, [sp, #16 * 13]
        ldp x28, x29, [sp, #16 * 14]
        add sp, sp, #272
.endm

.macro usavereg
        sub sp, sp, #272

        stp x0, x1, [sp, #16 * 0]
        stp x2, x3, [sp, #16 * 1]
        stp x4, x5, [sp, #16 * 2]
        stp x6, x7, [sp, #16 * 3]
        stp x8, x9, [sp, #16 * 4]
        stp x10, x11, [sp, #16 * 5]
        stp x12, x13, [sp, #16 * 6]
        stp x14, x15, [sp, #16 * 7]
        stp x16, x17, [sp, #16 * 8]
        stp x18, x19, [sp, #16 * 9]
        stp x20, x21, [sp, #16 * 10]
        stp x22, x23, [sp, #16 * 11]
        stp x24, x25, [sp, #16 * 12]
        stp x26, x27, [sp, #16 * 13]
        stp x28, x29, [sp, #16 * 14]

        mrs x9, elr_el1
        mrs x10, spsr_el1
        mrs x11, sp_el0

        stp x30, x9, [sp, #16 * 15]
        stp x10, x11, [sp, #16 * 16]
.endm

.macro urestorereg
        ldp x30, x9, [sp, #16 * 15]
        ldp x10, x11, [sp, #16 * 16]

        msr elr_el1, x9
        msr spsr_el1, x10
        msr sp_el0, x11

        ldp x0, x1, [sp, #16 * 0]
        ldp x2, x3, [sp, #16 * 1]
        ldp x4, x5, [sp, #16 * 2]
        ldp x6, x7, [sp, #16 * 3]
        ldp x8, x9, [sp, #16 * 4]
        ldp x10, x11, [sp, #16 * 5]
        ldp x12, x13, [sp, #16 * 6]
        ldp x14, x15, [sp, #16 * 7]
        ldp x16, x17, [sp, #16 * 8]
        ldp x18, x19, [sp, #16 * 9]
        ldp x20, x21, [sp, #16 * 10]
        ldp x22, x23, [sp, #16 * 11]
        ldp x24, x25, [sp, #16 * 12]
        ldp x26, x27, [sp, #16 * 13]
        ldp x28, x29, [sp, #16 * 14]

        add sp, sp, #272
.endm


.global alltraps
.balign 0x800
alltraps:
// Current EL with sp0
  b badtrap
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap

// Current EL with spx
.balign 0x80
  b el1sync
.balign 0x80
  b el1irq
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap

// Lower EL using aarch64
.balign 0x80
  b el0sync
.balign 0x80
  b el0irq
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap

// Lower EL using aarch32
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap
.balign 0x80
  b badtrap

badtrap:
        savereg
        mov x0, sp
        bl  kernel_intr_handler 
        b   .

el1sync:
        savereg
        mov x0, sp
        bl  kernel_abort_handler 
        b   .
      
el1irq:
        usavereg
        mov x0, sp
        bl intr_irq_dispatch

        urestorereg
      
        eret
      
el0sync:
        usavereg
        mov x0, sp
        bl syscall_arch_handler
      
        urestorereg
      
        eret

el0irq:
        usavereg
        mov x0, sp
        bl intr_irq_dispatch
      
.global trap_return
trap_return:
        urestorereg

        eret