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
 * @file sys_semaphore.c
 * @brief support semaphore for userland
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.05.29
 */

#include "ksemaphore.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

extern bool ksemaphore_wait(struct XiziSemaphorePool* sem_pool, struct Thread* thd, sem_id_t sem_id);
int sys_semaphore(sys_sem_option op, int param)
{
    bool ret = false;
    switch (op) {
    case SYS_SEM_NEW: {
        // here, param is treat as val
        return ksemaphore_alloc(&xizi_task_manager.semaphore_pool, param);
    }
    case SYS_SEM_FREE: {
        // here, param is treat as sem_id
        ret = ksemaphore_free(&xizi_task_manager.semaphore_pool, param);
        break;
    }
    case SYS_SEM_SIGNAL: {
        ret = ksemaphore_signal(&xizi_task_manager.semaphore_pool, param);
        break;
    }
    case SYS_SEM_WAIT: {
        ret = ksemaphore_wait(&xizi_task_manager.semaphore_pool, cur_cpu()->task, param);
        break;
    }
    }

    if (LIKELY(ret)) {
        return 0;
    }
    return -1;
}