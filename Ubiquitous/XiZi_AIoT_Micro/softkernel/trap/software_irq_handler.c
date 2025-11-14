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
 * @file software_irq_handler.c
 * @brief syscall distributor
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: software_irq_handler.c
Description: syscall distributor
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "core.h"
#include "trap_common.h"

#include "assert.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

static struct SwiDispatcherRightGroup right_group;
static struct XiziTrapDriver* p_intr_driver = NULL;

bool swi_distributer_init(struct SwiDispatcherRightGroup* _right_group)
{
    right_group = *_right_group;
    p_intr_driver = AchieveResource(&_right_group->intr_driver_tag);
    return p_intr_driver != NULL;
}

#ifndef __riscv
extern void context_switch(struct context**, struct context*);
#else
extern void context_switch(struct context*, struct context*);
#endif
void software_irq_dispatch(struct trapframe* tf)
{
    xizi_enter_kernel();
    assert(p_intr_driver != NULL);

    // get current task
    struct Thread* cur_task = cur_cpu()->task;
    /// @todo: Handle dead task
    int syscall_num = -1;
    if (cur_task && cur_task->snode.state != DEAD) {
        cur_task->thread_context.trapframe = tf;
        // call syscall
        int ret = arch_syscall(cur_task->thread_context.trapframe, &syscall_num);
        arch_set_return(tf, ret);
    }

    assert(cur_cpu()->task == cur_task && cur_task->snode.state == RUNNING);
    if (!queue_is_empty(&cur_task->snode.state_trans_signal_queue)) {
#ifndef __riscv
        context_switch(&cur_task->thread_context.context, cur_cpu()->scheduler);
#else
        struct CPU* cpu = cur_cpu();
        context_switch(cur_task->thread_context.context, &cpu->scheduler);
#endif
    }
    if (syscall_num == SYSCALL_EXIT) {
        panic("Exit reaches");
    }

    assert(cur_task == cur_cpu()->task);
    xizi_leave_kernel();
}
