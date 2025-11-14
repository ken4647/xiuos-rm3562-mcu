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
 * @file syscall.c
 * @brief syscall distributor
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: syscall.c
Description: syscall distributor
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "log.h"
#include "multicores.h"
#include "trap_common.h"

#include "syscall.h"

int syscall(int sys_num, uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4)
{
    int ret = -1;

    switch (sys_num) {
    case SYSCALL_TEST:
        ret = 0;
        break;
    case SYSCALL_SPAWN:
        ret = sys_spawn((char*)param1, (char*)param2, (char**)param3);
        break;
    case SYSCALL_THREAD:
        ret = sys_thread((uintptr_t)param1, (char*)param2, (char**)param3);
        break;
    case SYSCALL_EXIT:
        ret = sys_exit(cur_cpu()->task);
        break;
    case SYSCALL_YIELD:
        ret = sys_yield((task_yield_reason)param1);
        break;
    case SYSCALL_SERVER:
        ret = sys_register_as_server((char*)param1);
        break;
    case SYSCALL_SESSION:
        ret = sys_connect_session((char*)param1, (int)param2, (struct Session*)param3);
        break;
    case SYSCALL_POLL_SESSION:
        ret = sys_poll_session((struct Session*)param1, (int)param2);
        break;
    case SYSCALL_CLOSE_SESSION:
        ret = sys_close_session(cur_cpu()->task, (struct Session*)param1);
        break;
    case SYSCALL_SYS_STATE:
        ret = sys_state(param1, (sys_state_info*)param2);
        break;
    case SYSCALL_MMAP:
        ret = sys_mmap_v2((uintptr_t*)param1, (uintptr_t*)param2, (int)param3, (sys_mmap_info*)param4);
        break;
    case SYSCALL_REGISTER_IRQ:
        ret = sys_register_irq((int)param1, (int)param2);
        break;
    case SYSCALL_KILL:
        ret = sys_kill((int)param1);
        break;
    case SYSCALL_SEMAPHORE:
        ret = sys_semaphore((sys_sem_option)param1, (int)param2);
        break;
    case SYSCALL_SLEEP:
        ret = sys_sleep((intptr_t)param1);
        break;
    case SYSCALL_WAIT_SESSION:
        ret = sys_wait_session((struct Session*)param1);
        break;
    default:
        ERROR("Unsurport syscall(%d) right now\n", sys_num);
        ret = -1;
        break;
    }

    return ret;
}