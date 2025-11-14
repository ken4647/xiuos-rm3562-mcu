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
 * @file sys_exit.c
 * @brief task exit syscall
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_exit.c
Description: task exit syscall
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "trap_common.h"

#include "actracer.h"
#include "assert.h"
#include "list.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

int sys_exit(struct Thread* ptask)
{
    assert(ptask != NULL);
    ptask->dead = true;
    // awake the task if it's a blocked task
    if (ptask->snode.state == BLOCKED || ptask->snode.state == SLEEPING) {
        THREAD_TRANS_STATE(ptask, TRANS_WAKING);
    }
    // yield current task in case it wants to exit itself
    THREAD_TRANS_STATE(cur_cpu()->task, READY);
    return 0;
}
