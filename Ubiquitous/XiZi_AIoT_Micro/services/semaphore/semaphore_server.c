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
#include "libserial.h"
#include "usyscall.h"

struct Semaphore {
    bool valid;
    int count;
};

struct Semaphore sem_pool[MAX_SUPPORT_SEMAPHORES];

int IPC_DO_SERVE_FUNC(Ipc_sem_create)(sem_t* sem, int* count)
{
    int idx = 0;
    for (idx = 0; idx < MAX_SUPPORT_SEMAPHORES; idx++) {
        if (!sem_pool[idx].valid) {
            break;
        }
    }

    if (idx == MAX_SUPPORT_SEMAPHORES) {
        return SEMAPHORE_ERR;
    }

    sem_pool[idx].valid = true;
    sem_pool[idx].count = *count;

    *sem = idx;
    return SEMAPHORE_SUC;
}


#define CHECK_SEM_RANGE(sem) \
    do { \
        if (*sem < 0 || *sem >= MAX_SUPPORT_SEMAPHORES) { \
            return SEMAPHORE_ERR; \
        } \
    } while (0)


#define CHECK_SEM_RANGE_AND_VALID(sem) \
    do { \
        CHECK_SEM_RANGE(sem); \
        \
        if (!sem_pool[*sem].valid) { \
            return SEMAPHORE_ERR; \
        } \
    } while (0)


int IPC_DO_SERVE_FUNC(Ipc_sem_delete)(sem_t* sem)
{
    CHECK_SEM_RANGE_AND_VALID(sem);

    sem_pool[*sem].valid = false;
    return SEMAPHORE_SUC;
}

int IPC_DO_SERVE_FUNC(Ipc_sem_wait)(sem_t* sem, int* timeout)
{
    CHECK_SEM_RANGE(sem);

    /// @todo support timeout
    // return if sem is freed(no valid) or sem count is sufficient
    while (sem_pool[*sem].valid && sem_pool[*sem].count <= 0)
        ;

    sem_pool[*sem].count--;
    return SEMAPHORE_SUC;
}

int IPC_DO_SERVE_FUNC(Ipc_sem_signal)(sem_t* sem)
{
    CHECK_SEM_RANGE_AND_VALID(sem);

    sem_pool[*sem].count++;
    return SEMAPHORE_SUC;
}

IPC_SERVER_INTERFACE(Ipc_sem_create, 2);
IPC_SERVER_INTERFACE(Ipc_sem_delete, 1);
IPC_SERVER_INTERFACE(Ipc_sem_signal, 1);
IPC_SERVER_THREAD_INTERFACE(Ipc_sem_wait, 2);
IPC_SERVER_REGISTER_INTERFACES(IpcSemaphoreServer, 4, //
    Ipc_sem_create, Ipc_sem_delete, Ipc_sem_signal, Ipc_sem_wait);

int main(int argc, char** argv)
{
    /// @todo support custom server name
    static char default_server_name[] = "DefaultSemaphoreServer";

    char* server_name = default_server_name;
    if (argc == 2) {
        server_name = argv[1];
    }

    if (register_server(server_name) < 0) {
        printf("[%s] Register server failed.\n", server_name);
        exit(1);
    }

    for (int idx = 0; idx < MAX_SUPPORT_SEMAPHORES; idx++) {
        sem_pool[idx].valid = false;
    }

    ipc_server_loop(&IpcSemaphoreServer);

    // never reaching here
    exit(0);
    return 0;
}