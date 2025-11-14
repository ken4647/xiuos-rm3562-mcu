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
#include <stdint.h>

#include "libserial.h"
#include "usyscall.h"

enum {
    NR_THREADS = 16,
    LOOP_TIMES = 0x1000000,
};

static int sem_id = -1;
static uint32_t sum = 0;
static int nr_thds = NR_THREADS;

int do_calculation(int argc, char** argv)
{
    for (uint32_t i = 0; i < LOOP_TIMES; i++) {
        sum++;
    }

    printf("test semaphore thd signal.\n");
    semaphore_signal(sem_id);

    char* params[] = { "do_cal", NULL };
    if (nr_thds != 0) {
        int tid = thread(do_calculation, "test_sem_thd", params);
        nr_thds--;
    }

    exit(0);
    return 0;
}

int main(int argc, char** argv)
{
    printf("Test Semaphore.\n");
    sem_id = semaphore_new(0);
    if (sem_id < 0) {
        printf("new a kernel sem failed.\n");
        exit(1);
    }

    sum = 0;

    nr_thds = NR_THREADS;
    char* params[] = { "do_cal", NULL };
    if (nr_thds != 0) {
        int tid = thread(do_calculation, "test_sem_thd", params);
        nr_thds--;
    }

    for (int i = 0; i < NR_THREADS; i++) {
        semaphore_wait(sem_id);
    }

    printf("test thread sum after %d signal: 0x%x\n", NR_THREADS, sum);

    semaphore_free(sem_id);
    exit(0);
    return 0;
}
