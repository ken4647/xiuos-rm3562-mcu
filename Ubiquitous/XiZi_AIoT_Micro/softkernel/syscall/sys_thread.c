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
 * @file sys_spawn.c
 * @brief spawn a task
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_thread.c
Description: spawn a thread
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "actracer.h"
#include "assert.h"
#include "memspace.h"
#include "multicores.h"
#include "share_page.h"
#include "syscall.h"
#include "task.h"

int sys_new_thread(struct MemSpace* pmemspace, struct Thread* task, uintptr_t entry, char* name, char** argv)
{
    struct ThreadStackPointer loaded_sp = load_user_stack(pmemspace, argv);
    if (loaded_sp.stack_idx == -1) {
        ERROR("Uable to load params to memspace.\n");
        /* memspace is freed alone with free_thread() */
        struct TaskLifecycleOperations* tlo = GetSysObject(struct TaskLifecycleOperations, &xizi_task_manager.task_lifecycle_ops_tag);
        tlo->free_thread(task);
        return -1;
    }

    // init trapframe
    task->thread_context.user_stack_idx = loaded_sp.stack_idx;
    task->thread_context.uspace_stack_addr = USER_MEM_TOP - ((loaded_sp.stack_idx + 1) * USER_STACK_SIZE);
    task->thread_context.ustack_kvaddr = loaded_sp.user_stack_vaddr;
    arch_init_trapframe(task->thread_context.trapframe, 0, 0);
    arch_trapframe_set_sp_pc(task->thread_context.trapframe, loaded_sp.user_sp, (uintptr_t)entry);
    arch_set_main_params(task->thread_context.trapframe, loaded_sp.argc, loaded_sp.user_sp);

#ifdef __riscv
    arch_context_set_sp(task->thread_context.context, (uintptr_t)task->thread_context.trapframe);
#endif

    // init thread name
    char* last = NULL;
    for (last = name; *name; name++) {
        if (*name == '/') {
            last = name + 1;
        }
    }
    strncpy(task->name, last, sizeof(task->name) - 1);

    // init pcb schedule attributes
    task_into_ready(task);

    // thread init done by here
    if (pmemspace->thread_to_notify == NULL) {
        pmemspace->thread_to_notify = task;
    }

    return task->tid;
}

int sys_thread(uintptr_t entry, char* name, char** argv)
{
    struct Thread* cur_task = cur_cpu()->task;

    // use current task's memspace
    struct MemSpace* pmemspace = cur_task->memspace;

    struct TaskLifecycleOperations* tlo = GetSysObject(struct TaskLifecycleOperations, &xizi_task_manager.task_lifecycle_ops_tag);
    struct Thread* task = tlo->new_thread(pmemspace);
    if (UNLIKELY(!task)) {
        ERROR("Unable to new task control block.\n");
        return -1;
    }
    assert(!IS_DOUBLE_LIST_EMPTY(&pmemspace->thread_list_guard));

    return sys_new_thread(pmemspace, task, entry, name, argv);
}