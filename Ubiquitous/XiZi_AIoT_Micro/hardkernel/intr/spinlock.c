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
 * @file spinlock.c
 * @brief spinlock interfaces
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.23
 */
#include <stdint.h>
#include <string.h>

#include "spinlock.h"
#include "trap_common.h"

#include "assert.h"
#include "list.h"
#include "multicores.h"

struct lock_node {
    struct double_list_node node;
    int cpu_id;
};

static struct double_list_node lock_request_guard;
static struct lock_node core_lock_request[NR_CPU];
static struct spinlock request_lock;
bool module_spinlock_use_intr_init(void)
{
    for (int i = 0; i < NR_CPU; i++) {
        core_lock_request[i].cpu_id = i;
        doubleListNodeInit(&core_lock_request[i].node);
    }
    doubleListNodeInit(&lock_request_guard);
    spinlock_init(&request_lock, "requestlock");
    return true;
}

#define SPINLOCK_STATE_UNLOCK 0xFF

enum {
    SPINLOCK_LOCK_NOWAIT = 0,
    SPINLOCK_LOCK_WAITFOREVER = 0xFFFFFFFF,
};

__attribute__((optimize("O0"))) void spinlock_init(struct spinlock* lock, char* name)
{
    lock->owner_cpu = SPINLOCK_STATE_UNLOCK;
    strncpy(lock->name, name, 24);
}

extern int _spinlock_lock(struct spinlock* lock, uint32_t timeout);
extern int _spinlock_unlock(struct spinlock* lock);

__attribute__((optimize("O0"))) void spinlock_lock(struct spinlock* lock)
{
    int cur_cpu_id = cur_cpuid();
    if (lock->owner_cpu != SPINLOCK_STATE_UNLOCK && lock->owner_cpu == cur_cpu_id) {
        ERROR("spinlock %s lock double locked by core %d\n", lock->name, lock->owner_cpu);
        panic("");
    }

    struct double_list_node* p_lock_node = &core_lock_request[cur_cpu_id].node;
    _spinlock_lock(&request_lock, SPINLOCK_LOCK_WAITFOREVER);
    doubleListAddOnBack(p_lock_node, &lock_request_guard);
    _spinlock_unlock(&request_lock);

    while (lock_request_guard.next != p_lock_node)
        ;

    _spinlock_lock(lock, SPINLOCK_LOCK_WAITFOREVER);
}

__attribute__((optimize("O0"))) void spinlock_unlock(struct spinlock* lock)
{
    struct double_list_node* p_lock_node = &core_lock_request[cur_cpuid()].node;
    assert(lock_request_guard.next == p_lock_node);
    _spinlock_lock(&request_lock, SPINLOCK_LOCK_WAITFOREVER);
    _double_list_del(p_lock_node->prev, p_lock_node->next);
    _spinlock_unlock(&request_lock);

    if (_spinlock_unlock(lock) != 0) {
        ERROR("Core %d trying to unlock a lock belongs to %d.\n", cur_cpuid(), lock->owner_cpu);
    }
}

__attribute__((optimize("O0"))) bool spinlock_try_lock(struct spinlock* lock)
{
    int cur_cpu_id = cur_cpuid();
    if (lock->owner_cpu != SPINLOCK_STATE_UNLOCK && lock->owner_cpu == cur_cpu_id) {
        ERROR("spinlock %s lock double locked by core %d\n", lock->name, lock->owner_cpu);
        panic("");
    }

    struct double_list_node* p_lock_node = &core_lock_request[cur_cpu_id].node;
    _spinlock_lock(&request_lock, SPINLOCK_LOCK_WAITFOREVER);
    doubleListAddOnBack(p_lock_node, &lock_request_guard);
    if (lock_request_guard.next != p_lock_node) {
        _double_list_del(p_lock_node->prev, p_lock_node->next);
        _spinlock_unlock(&request_lock);
        return false;
    }
    _spinlock_unlock(&request_lock);
    _spinlock_lock(lock, SPINLOCK_LOCK_WAITFOREVER);

    return true;
}

bool is_spinlock_hold_by_current_cpu(struct spinlock* lock)
{
    return lock->owner_cpu == cur_cpuid();
}