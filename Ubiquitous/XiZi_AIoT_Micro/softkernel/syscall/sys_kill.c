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
 * @file sys_kill.c
 * @brief task exit syscall
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.03.19
 */

/*************************************************
File name: sys_kill.c
Description: task kill syscall
Others:
History:
1. Date: 2023-03-19
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "task.h"
#include "trap_common.h"

static bool kill_succ;

extern int sys_exit(struct Thread* ptask);
static bool kill_task(RbtNode* node, void* id)
{
    struct ScheduleNode* snode = (struct ScheduleNode*)node->data;
    struct Thread* thd = snode->pthd;
    tid_t target_id = *(tid_t*)id;

    if (thd->tid == target_id) {
        sys_exit(thd);
        kill_succ = true;
        return false;
    }

    return true;
}

extern int sys_exit(struct Thread* task);
int sys_kill(int id)
{
    kill_succ = false;
    for (int pool_id = 0; pool_id < NR_STATE; pool_id++) {
        rbt_traverse(&g_scheduler.snode_state_pool[pool_id], kill_task, (void*)&id);
    }

    if (kill_succ) {
        return 0;
    }
    return -1;
}