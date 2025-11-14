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

#include "libsemaphore.h"

IPC_INTERFACE(Ipc_sem_create, 2, sem, count, sizeof(sem_t), sizeof(int));
sem_err_t sem_create(struct Session* session, sem_t* sem, int count)
{
    return IPC_CALL(Ipc_sem_create)(session, sem, &count);
}

IPC_INTERFACE(Ipc_sem_delete, 1, sem, sizeof(sem_t));
sem_err_t sem_delete(struct Session* session, sem_t* sem)
{
    return IPC_CALL(Ipc_sem_delete)(session, sem);
}

IPC_INTERFACE(Ipc_sem_signal, 1, sem, sizeof(sem_t));
int sem_signal(struct Session* session, sem_t* sem)
{
    return IPC_CALL(Ipc_sem_signal)(session, sem);
}

IPC_INTERFACE(Ipc_sem_wait, 2, sem, timeout, sizeof(sem_t), sizeof(int));
int sem_wait(struct Session* session, sem_t* sem, int timeout)
{
    return IPC_CALL(Ipc_sem_wait)(session, sem, &timeout);
}