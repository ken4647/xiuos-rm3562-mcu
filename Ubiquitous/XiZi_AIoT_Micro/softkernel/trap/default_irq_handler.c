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
 * @file default_irq_handler.c
 * @brief irq distributor
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: default_irq_handler.c
Description: irq distributor
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stddef.h>

#include "core.h"
#include "trap_common.h"

#include "assert.h"
#include "log.h"
#include "multicores.h"
#include "task.h"

static struct IrqDispatcherRightGroup right_group;
static struct XiziTrapDriver* p_intr_driver = NULL;
bool intr_distributer_init(struct IrqDispatcherRightGroup* _right_group)
{
    right_group = *_right_group;
    p_intr_driver = AchieveResource(&_right_group->intr_driver_tag);
    return p_intr_driver != NULL;
}

void default_interrupt_routine(int irq)
{
    /* default handler borrow the rights of dispatcher */
    ///@todo Support other cores. (currently assume that CPU_0 is used)
    ERROR("Interrupt %d has been asserted\n", irq);
}

#ifndef __riscv
extern void context_switch(struct context**, struct context*);
#else
extern void context_switch(struct context*, struct context*);
#endif
void intr_irq_dispatch(struct trapframe* tf)
{
    xizi_enter_kernel();

    // enter irq
    assert(p_intr_driver != NULL);
    uintptr_t int_info = 0;
    if ((int_info = p_intr_driver->hw_before_irq()) == 0) {
        goto intr_leave_interrupt;
    }

    struct Thread* current_task = cur_cpu()->task;
    assert(current_task != NULL);
    current_task->thread_context.trapframe = tf;

    int cpu = cur_cpuid();
    assert(cpu >= 0 && cpu < NR_CPU);
    unsigned irq = p_intr_driver->hw_cur_int_num(int_info);

    // distribute irq
    irq_handler_t isr = p_intr_driver->sw_irqtbl[irq].handler;
    if (isr != NULL) {
        isr(irq, tf, NULL);
    } else {
        default_interrupt_routine(irq);
    }

    // finish irq.
    p_intr_driver->hw_after_irq(int_info);

    assert(cur_cpu()->task == current_task && current_task->snode.state == RUNNING);
    if (!queue_is_empty(&current_task->snode.state_trans_signal_queue)) {
#ifndef __riscv
        context_switch(&current_task->thread_context.context, cur_cpu()->scheduler);
#else
        context_switch(current_task->thread_context.context, &cur_cpu()->scheduler);
#endif
    }
    assert(current_task == cur_cpu()->task);

intr_leave_interrupt:
    xizi_leave_kernel();
}

__attribute__((always_inline)) inline void xizi_enter_kernel()
{
    /// @warning trampoline is responsible for closing interrupt
    spinlock_lock(&whole_kernel_lock);
}

__attribute__((always_inline)) inline bool xizi_try_enter_kernel()
{
    /// @warning trampoline is responsible for closing interrupt
    if (spinlock_try_lock(&whole_kernel_lock)) {
        return true;
    }

    return false;
}

__attribute__((always_inline)) inline void xizi_leave_kernel()
{
    /// @warning trampoline is responsible for eabling interrupt by using user's state register
    spinlock_unlock(&whole_kernel_lock);
}
