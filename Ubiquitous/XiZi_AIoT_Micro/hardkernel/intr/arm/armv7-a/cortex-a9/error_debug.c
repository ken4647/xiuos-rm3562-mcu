/* Copyright (c) 2006-2018 Frans Kaashoek, Robert Morris, Russ Cox,
 *                        Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/**
 * @file error_debug.c
 * @brief handle program abort
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.23
 */

/*************************************************
File name: error_debug.c
Description: handle program abort
Others:
History:
1. Date: 2023-11-23
Author: AIIT XUOS Lab
Modification:
1. Take only armv7 abort reason part(_abort_reason).
2. Modify iabort and dabort handler(in dabort_handler() and iabort_handler())
*************************************************/
#include "core.h"
#include "memlayout.h"
#include "spinlock.h"
#include "trap_common.h"

#include "assert.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

void dump_tf(struct trapframe* tf)
{
    KPrintf("sp_usr:  0x%x\n", tf->sp_usr);
    KPrintf("lr_usr:  0x%x\n", tf->lr_usr);
    KPrintf("lr_svc:  0x%x\n", tf->lr_svc);
    KPrintf("   spsr: 0x%x\n", tf->spsr);
    KPrintf("     r0: 0x%x\n", tf->r0);
    KPrintf("     r1: 0x%x\n", tf->r1);
    KPrintf("     r2: 0x%x\n", tf->r2);
    KPrintf("     r3: 0x%x\n", tf->r3);
    KPrintf("     r4: 0x%x\n", tf->r4);
    KPrintf("     r5: 0x%x\n", tf->r5);
    KPrintf("     r6: 0x%x\n", tf->r6);
    KPrintf("     r7: 0x%x\n", tf->r7);
    KPrintf("     r8: 0x%x\n", tf->r8);
    KPrintf("     r9: 0x%x\n", tf->r9);
    KPrintf("    r10: 0x%x\n", tf->r10);
    KPrintf("    r11: 0x%x\n", tf->r11);
    KPrintf("    r12: 0x%x\n", tf->r12);
    KPrintf("     pc: 0x%x\n", tf->pc);
}

void dabort_reason(struct trapframe* r)
{
    uint32_t fault_status, dfa;
    __asm__ __volatile__("mrc p15, 0, %0, c5, c0, 0" : "=r"(fault_status)::);
    __asm__ __volatile__("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfa)::);
    LOG("program counter: 0x%x caused\n", r->pc);
    LOG("data abort at 0x%x, status 0x%x\n", dfa, fault_status);

    if ((fault_status & 0xd) == 0x1) // Alignment failure
        KPrintf("reason: alignment\n");
    else if ((fault_status & 0xd) == 0x5) // External abort "on translation"
        KPrintf("reason: ext. abort on trnslt.\n");
    else if ((fault_status & 0xd) == 0x5) // Translation
        KPrintf("reason: sect. translation\n");
    else if ((fault_status & 0xd) == 0x9) // Domain
        KPrintf("reason: sect. domain\n");
    else if ((fault_status & 0xd) == 0xd) // Permission
        KPrintf("reason: sect. permission\n");
    else if ((fault_status & 0xd) == 0x8) // External abort
        KPrintf("reason: ext. abort\n");
    else
        KPrintf("reason: unknown???\n");

    dump_tf(r);
}

void iabort_reason(struct trapframe* r)
{
    uint32_t fault_status, ifa;
    __asm__ __volatile__("mrc p15, 0, %0, c5, c0, 1" : "=r"(fault_status)::);
    __asm__ __volatile__("mrc p15, 0, %0, c6, c0, 2" : "=r"(ifa)::);
    LOG("prefetch abort at 0x%x, status 0x%x\n", ifa, fault_status);

    if ((fault_status & 0xd) == 0x1) // Alignment failure
        KPrintf("reason: alignment\n");
    else if ((fault_status & 0xd) == 0x5) // External abort "on translation"
        KPrintf("reason: ext. abort on trnslt.\n");
    else if ((fault_status & 0xd) == 0x5) // Translation
        KPrintf("reason: sect. translation\n");
    else if ((fault_status & 0xd) == 0x9) // Domain
        KPrintf("reason: sect. domain\n");
    else if ((fault_status & 0xd) == 0xd) // Permission
        KPrintf("reason: sect. permission\n");
    else if ((fault_status & 0xd) == 0x8) // External abort
        KPrintf("reason: ext. abort\n");
    else
        KPrintf("reason: unknown???\n");

    dump_tf(r);
}

void handle_undefined_instruction(struct trapframe* tf)
{
    // unimplemented trap handler
    ERROR("undefined instruction at %x\n", tf->pc);
    xizi_enter_kernel();
    panic("");
}

void handle_reserved(void)
{
    // unimplemented trap handler
    ERROR("Unimplemented Reserved\n");
    xizi_enter_kernel();
    panic("");
}

void handle_fiq(void)
{
    ERROR("Unimplemented FIQ\n");
    xizi_enter_kernel();
    panic("");
}