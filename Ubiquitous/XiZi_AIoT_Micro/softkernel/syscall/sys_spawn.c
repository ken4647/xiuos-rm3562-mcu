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
File name: sys_spawn.c
Description: spawn a task
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
#include "share_page.h"
#include "syscall.h"
#include "task.h"

extern int sys_new_thread(struct MemSpace* pmemspace, struct Thread* task, uintptr_t entry, char* name, char** argv);
int sys_spawn(char* img_start, char* name, char** argv)
{
    // alloc a new memspace
    struct MemSpace* pmemspace = alloc_memspace(name);
    if (pmemspace == NULL) {
        return -1;
    }

    // load memspace
    uintptr_t* entry = load_memspace(pmemspace, img_start);
    if (pmemspace->pgdir.pd_addr == NULL) {
        ERROR("Loading memspace from %016x failed.\n", img_start);
        free_memspace(pmemspace);
        return -1;
    }

    // alloc a new pcb
    struct TaskLifecycleOperations* tlo = GetSysObject(struct TaskLifecycleOperations, &xizi_task_manager.task_lifecycle_ops_tag);
    struct Thread* new_task_cb = tlo->new_thread(pmemspace);
    if (UNLIKELY(!new_task_cb)) {
        ERROR("Unable to new task control block %x.\n");
        // error task allocation may free memspace before hand
        // @todo use task ref map to handle this scene
        if (NULL != pmemspace->tag.meta) {
            free_memspace(pmemspace);
        }

        return -1;
    }
    assert(!IS_DOUBLE_LIST_EMPTY(&pmemspace->thread_list_guard));

    return sys_new_thread(pmemspace, new_task_cb, (uintptr_t)entry, name, argv);
}