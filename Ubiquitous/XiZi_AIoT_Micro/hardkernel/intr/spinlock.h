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
 * @file spinlock.h
 * @brief spinlock header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: spinlock.h
Description: spinlock header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define STACK_DEPTH 32

struct spinlock { // Mutex.
    volatile uint32_t owner_cpu; // 1 for locked, 0 for unlocked
    char name[28]; // The call stack (an array of program counters)
} __attribute__((aligned(32)));

bool module_spinlock_use_intr_init(void);
void spinlock_init(struct spinlock* lock, char* name);
void spinlock_lock(struct spinlock* lock);
void spinlock_unlock(struct spinlock* lock);
bool spinlock_try_lock(struct spinlock* lock);
bool is_spinlock_hold_by_current_cpu(struct spinlock* lock);