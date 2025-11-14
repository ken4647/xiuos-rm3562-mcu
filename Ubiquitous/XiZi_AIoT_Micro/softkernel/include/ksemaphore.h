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
 * @file semaphore.h
 * @brief semaphore implementation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.05.28
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "object_allocator.h"
#include "rbtree.h"

typedef uintptr_t sem_id_t;
typedef int32_t sem_val_t;

enum {
    INVALID_SEM_ID = 0,
};

/// @warning this is no in use
enum {
    KSEM_NOWAIT = -1,
};

struct ksemaphore {
    sem_id_t id;
    sem_val_t val;
    /* list of waiting threads */
    RbtTree wait_thd_tree;
    /* list to manage semaphores */
    /// @todo Use RB-Tree to manage all semaphores
    struct double_list_node sem_list_node;
};

struct XiziSemaphorePool {
    sem_id_t next_sem_id;
    struct slab_allocator allocator;
    struct double_list_node sem_list_guard;
    RbtTree sem_pool_map;
    sem_val_t nr_sem;
};

void semaphore_pool_init(struct XiziSemaphorePool* sem_pool);
sem_id_t ksemaphore_alloc(struct XiziSemaphorePool* sem_pool, sem_val_t val);
bool ksemaphore_free(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id);
bool ksemaphore_signal(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id);
bool ksemaphore_signal_no_wake(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id);

bool ksemaphore_consume(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id, sem_val_t decre);
