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
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libipc.h"

#define MAX_SUPPORT_SEMAPHORES NR_MAX_SESSION

typedef uint32_t sem_t;
typedef int sem_err_t;

enum {
    SEMAPHORE_ERR = -1,
    SEMAPHORE_SUC = 1,
};

IPC_SERVICES(IpcSemaphoreServer, Ipc_sem_create, Ipc_sem_delete, Ipc_sem_signal, Ipc_sem_wait);

sem_err_t sem_create(struct Session* session, sem_t* sem, int count);
sem_err_t sem_delete(struct Session* session, sem_t* sem);
int sem_signal(struct Session* session, sem_t* sem);
int sem_wait(struct Session* session, sem_t* sem, int timeout);