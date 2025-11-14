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
 * @file multicores.h
 * @brief cpu header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: multicores.h
Description: cpu header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "core.h"
#include "spinlock.h"
#include "trap_common.h"

struct CPU {
    int cpuid;

    struct Thread* task;
#ifndef __riscv
    struct context* scheduler;
#else
    struct context scheduler;
#endif
};

extern struct CPU global_cpus[NR_CPU];

static inline struct CPU* cur_cpu(void)
{
    return &global_cpus[cur_cpuid()];
}

extern struct spinlock whole_kernel_lock;

void xizi_enter_kernel();
bool xizi_try_enter_kernel();
void xizi_leave_kernel();
bool xizi_is_in_kernel();