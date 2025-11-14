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
 * @file abort_handler.c
 * @brief handle program abort
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.23
 */

/*************************************************
File name: abort_handler.c
Description: handle program abort
Others:
History:
1. Date: 2023-11-23
Author: AIIT XUOS Lab
Modification:
1. Modify iabort and dabort handler(in dabort_handler() and iabort_handler())
*************************************************/
#include "core.h"
#include "memlayout.h"
#include "spinlock.h"
#include "trap_common.h"

#include "assert.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

#ifndef __riscv
extern void context_switch(struct context**, struct context*);
#else
extern void context_switch(struct context*, struct context*);
#endif
__attribute__((optimize("O0"))) void dabort_handler(struct trapframe* r)
{
#ifndef __riscv
    if (r->pc >= DEV_VRTMEM_BASE && is_spinlock_hold_by_current_cpu(&whole_kernel_lock)) {
        assert(is_spinlock_hold_by_current_cpu(&whole_kernel_lock));
        ERROR("dabort in kernel, current task: %s\n", cur_cpu()->task == NULL ? "NULL" : cur_cpu()->task->name);
        dabort_reason(r);
        panic("data abort exception\n");
    }
#else
    if (r->epc >= DEV_VRTMEM_BASE && is_spinlock_hold_by_current_cpu(&whole_kernel_lock)) {
        assert(is_spinlock_hold_by_current_cpu(&whole_kernel_lock));
        ERROR("dabort in kernel, current task: %s\n", cur_cpu()->task == NULL ? "NULL" : cur_cpu()->task->name);
        dabort_reason(r);
        panic("data abort exception\n");
    }
#endif

    struct Thread* cur_task = cur_cpu()->task;
    ERROR("dabort in user space: %s\n", cur_task->name);
    dabort_reason(r);

    xizi_enter_kernel();
    sys_exit(cur_task);
#ifndef __riscv
    context_switch(&cur_task->thread_context.context, cur_cpu()->scheduler);
#else
    context_switch(cur_task->thread_context.context, &cur_cpu()->scheduler);
#endif
    panic("dabort end should never be reashed.\n");
}

__attribute__((optimize("O0"))) void iabort_handler(struct trapframe* r)
{
#ifndef __riscv
    if (r->pc >= DEV_VRTMEM_BASE && is_spinlock_hold_by_current_cpu(&whole_kernel_lock)) {
        assert(is_spinlock_hold_by_current_cpu(&whole_kernel_lock));
        ERROR("iabort in kernel, current task: %s\n", cur_cpu()->task == NULL ? "NULL" : cur_cpu()->task->name);
        iabort_reason(r);
        panic("kernel prefetch abort exception\n");
    }
#else
    if (r->epc >= DEV_VRTMEM_BASE && is_spinlock_hold_by_current_cpu(&whole_kernel_lock)) {
        assert(is_spinlock_hold_by_current_cpu(&whole_kernel_lock));
        ERROR("iabort in kernel, current task: %s\n", cur_cpu()->task == NULL ? "NULL" : cur_cpu()->task->name);
        iabort_reason(r);
        panic("kernel prefetch abort exception\n");
    }
#endif

    struct Thread* cur_task = cur_cpu()->task;
    ERROR("iabort in user space: %s\n", cur_task->name);
    iabort_reason(r);

    xizi_enter_kernel();
    sys_exit(cur_task);
#ifndef __riscv
    context_switch(&cur_task->thread_context.context, cur_cpu()->scheduler);
#else
    context_switch(cur_task->thread_context.context, &cur_cpu()->scheduler);
#endif
    panic("iabort end should never be reashed.\n");
}
