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
 * @file sys_sleep.c
 * @brief task sleep
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_sleep.c
Description:
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "multicores.h"
#include "syscall.h"
#include "task.h"

#include "assert.h"

int sys_sleep(intptr_t ms)
{
    struct Thread* cur_task = cur_cpu()->task;
    cur_task->snode.sleep_context.remain_ms = ms;
    THREAD_TRANS_STATE(cur_task, SLEEPING);

    return 0;
}