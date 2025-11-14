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
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.4.25
 */

/*************************************************
File name: error_debug.c
Description: handle program abort
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. Take only armv8 abort reason part(_abort_reason).
2. Modify iabort and dabort handler(in dabort_handler() and iabort_handler())
*************************************************/
#include <stddef.h>
#include <stdint.h>

#include "exception_registers.h"

#include "assert.h"
#include "core.h"
#include "log.h"
#include "multicores.h"
#include "task.h"
#include "trap_common.h"

void dump_tf(struct trapframe* tf)
{
    KPrintf("   sp:   0x%016lx\n", tf->sp);
    KPrintf("   pc:   0x%016lx\n", tf->pc);
    KPrintf("   spsr: 0x%016lx\n", tf->spsr);
    KPrintf("     x0: 0x%016lx\n", tf->x0);
    KPrintf("     x1: 0x%016lx\n", tf->x1);
    KPrintf("     x2: 0x%016lx\n", tf->x2);
    KPrintf("     x3: 0x%016lx\n", tf->x3);
    KPrintf("     x4: 0x%016lx\n", tf->x4);
    KPrintf("     x5: 0x%016lx\n", tf->x5);
    KPrintf("     x6: 0x%016lx\n", tf->x6);
    KPrintf("     x7: 0x%016lx\n", tf->x7);
    KPrintf("     x8: 0x%016lx\n", tf->x8);
    KPrintf("     x9: 0x%016lx\n", tf->x9);
    KPrintf("    x10: 0x%016lx\n", tf->x10);
    KPrintf("    x11: 0x%016lx\n", tf->x11);
    KPrintf("    x12: 0x%016lx\n", tf->x12);
    KPrintf("    x13: 0x%016lx\n", tf->x13);
    KPrintf("    x14: 0x%016lx\n", tf->x14);
    KPrintf("    x15: 0x%016lx\n", tf->x15);
    KPrintf("    x16: 0x%016lx\n", tf->x16);
    KPrintf("    x17: 0x%016lx\n", tf->x17);
    KPrintf("    x18: 0x%016lx\n", tf->x18);
    KPrintf("    x19: 0x%016lx\n", tf->x19);
    KPrintf("    x20: 0x%016lx\n", tf->x20);
    KPrintf("    x21: 0x%016lx\n", tf->x21);
    KPrintf("    x22: 0x%016lx\n", tf->x22);
    KPrintf("    x23: 0x%016lx\n", tf->x23);
    KPrintf("    x24: 0x%016lx\n", tf->x24);
    KPrintf("    x25: 0x%016lx\n", tf->x25);
    KPrintf("    x26: 0x%016lx\n", tf->x26);
    KPrintf("    x27: 0x%016lx\n", tf->x27);
    KPrintf("    x28: 0x%016lx\n", tf->x28);
    KPrintf("    x29: 0x%016lx\n", tf->x29);
    KPrintf("    x30: 0x%016lx\n", tf->x30);
}

void dabort_reason(struct trapframe* r)
{
    uint32_t fault_status, fault_address;
    __asm__ __volatile__("mrs %0, esr_el1" : "=r"(fault_status));
    __asm__ __volatile__("mrs %0, far_el1" : "=r"(fault_address));
    w_esr_el1(0);
    LOG("program counter: 0x%016lx caused\n", r->pc);
    LOG("data abort at 0x%016lx, status 0x%016lx\n", fault_address, fault_status);
    if ((fault_status & 0x3f) == 0x21) // Alignment failure
        KPrintf("reason: alignment\n");
    else if ((fault_status & 0x3f) == 0x4) // Translation fault, level 0
        KPrintf("reason: sect. translation level 0\n");
    else if ((fault_status & 0x3f) == 0x5) // Translation fault, level 1
        KPrintf("reason: sect. translation level 1\n");
    else if ((fault_status & 0x3f) == 0x6) // Translation fault, level 2
        KPrintf("reason: sect. translation level 2\n");
    else if ((fault_status & 0x3f) == 0x7) // Translation fault, level 3
        KPrintf("reason: sect. translation level 3\n");
    else if ((fault_status & 0x3f) == 0x3d) // Section Domain fault
        KPrintf("reason: sect. domain\n");
    else if ((fault_status & 0x3f) == 0xd) // Permission level 1
        KPrintf("reason: sect. permission level 1\n");
    else if ((fault_status & 0x3f) == 0xe) // Permission level 2
        KPrintf("reason: sect. permission level 2\n");
    else if ((fault_status & 0x3f) == 0xf) // Permission level 3
        KPrintf("reason: sect. permission level 3\n");
    else if ((fault_status & 0x3f) == 0x14) // External abort
        KPrintf("reason: ext. abort\n");
    else if ((fault_status & 0x3f) == 0x9) // Access flag fault, level 1
        KPrintf("reason: sect. Access flag fault level 1\n");
    else if ((fault_status & 0x3f) == 0xa) // Access flag fault, level 2
        KPrintf("reason: sect. Access flag fault level 2\n");
    else if ((fault_status & 0x3f) == 0xb) // Access flag fault, level 3
        KPrintf("reason: sect. Access flag fault level 3\n");
    else
        KPrintf("reason: unknown???\n");

    dump_tf(r);
    return;
}

void iabort_reason(struct trapframe* r)
{
    uint32_t fault_status, fault_address;
    __asm__ __volatile__("mrs %0, esr_el1" : "=r"(fault_status));
    __asm__ __volatile__("mrs %0, far_el1" : "=r"(fault_address));
    LOG("program counter: 0x%016lx caused\n", r->pc);
    LOG("data abort at 0x%016lx, status 0x%016lx\n", fault_address, fault_status);
    w_esr_el1(0);
    if ((fault_status & 0x3f) == 0x21) // Alignment failure
        KPrintf("reason: alignment\n");
    else if ((fault_status & 0x3f) == 0x4) // Translation fault, level 0
        KPrintf("reason: sect. translation level 0\n");
    else if ((fault_status & 0x3f) == 0x5) // Translation fault, level 1
        KPrintf("reason: sect. translation level 1\n");
    else if ((fault_status & 0x3f) == 0x6) // Translation fault, level 2
        KPrintf("reason: sect. translation level 2\n");
    else if ((fault_status & 0x3f) == 0x7) // Translation fault, level 3
        KPrintf("reason: sect. translation level 3\n");
    else if ((fault_status & 0x3f) == 0x3d) // Section Domain fault
        KPrintf("reason: sect. domain\n");
    else if ((fault_status & 0x3f) == 0xd) // Permission level 1
        KPrintf("reason: sect. permission level 1\n");
    else if ((fault_status & 0x3f) == 0xe) // Permission level 2
        KPrintf("reason: sect. permission level 2\n");
    else if ((fault_status & 0x3f) == 0xf) // Permission level 3
        KPrintf("reason: sect. permission level 3\n");
    else if ((fault_status & 0x3f) == 0x14) // External abort
        KPrintf("reason: ext. abort\n");
    else if ((fault_status & 0x3f) == 0x9) // Access flag fault, level 1
        KPrintf("reason: sect. Access flag fault level 1\n");
    else if ((fault_status & 0x3f) == 0xa) // Access flag fault, level 2
        KPrintf("reason: sect. Access flag fault level 2\n");
    else if ((fault_status & 0x3f) == 0xb) // Access flag fault, level 3
        KPrintf("reason: sect. Access flag fault level 3\n");
    else
        KPrintf("reason: unknown???\n");

    dump_tf(r);
    return;
}