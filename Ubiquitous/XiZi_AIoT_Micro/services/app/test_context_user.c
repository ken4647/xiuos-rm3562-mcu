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

#include "libserial.h"
#include "usyscall.h"

static inline uintptr_t arch_curr_tick()
{
    uint64_t x;
    __asm__ volatile("mrs %0, cntpct_el0" : "=r"(x));
    return x;
}

int main(int argc, char* argv[])
{
    uintptr_t loop = 1000;
    uintptr_t tick_total = 0;
    for (uintptr_t i = 0; i < loop; i++) {
        uintptr_t tick_1 = sys_test();
        uintptr_t tick_2 = sys_test();
        // printf("TICK before: %lu\n", tick_1);
        // printf("TICK after: %lu\n", tick_2);
        // printf("TICK diff: %lu\n", tick_2 - tick_1);
        tick_total += tick_2 - tick_1;
    }

    printf("TICK DIFF: %lu\n", tick_total / loop);

    printf("TEST TICK: %lu\n", arch_curr_tick());

    exit(0);
    return 0;
}
