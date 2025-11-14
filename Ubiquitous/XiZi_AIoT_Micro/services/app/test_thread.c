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

#include "libfs.h"
#include "libsemaphore.h"
#include "libserial.h"
#include "usyscall.h"

static int global_value;
static sem_t sem;
static char sem_server_name[] = "SemTestServer";

int sub_thread_entry(int argc, char** argv)
{
    for (int i = 0; i < 100; i++) {
        global_value++;
    }

    // sub thread connect to semaphore server
    printf("Thread signal sem.\n");
    semaphore_signal(sem);

    exit(0);
    return 0;
}

static char sem_file_name[] = "/semaphore_server";
int main(int argc, char** argv)
{
    global_value = 0;

    printf("Test Create Semaphore.\n");
    printf("Create new sem.\n");
    sem = semaphore_new(0);

    printf("Create new thread.\n");
    char* task_param[2] = { "add_gval", NULL };
    int nr_thread = 0;
    for (int i = 0; i < 128; i++) {
        int tid = thread(sub_thread_entry, "add_gval", task_param);
        if (tid > 0) {
            nr_thread++;
        }
    }

    printf("Main thread waiting for sem for %d times.\n", nr_thread);
    for (int i = 0; i < nr_thread; i++) {
        int sem_wait_ret = semaphore_wait(sem);
    }
    printf("Main thread sem %d wait return, global val: %d.\n", sem, global_value);

    semaphore_free(sem);
    exit(0);
    return 0;
}