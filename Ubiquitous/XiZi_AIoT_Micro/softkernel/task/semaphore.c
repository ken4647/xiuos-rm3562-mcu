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
 * @file semaphore.c
 * @brief semaphore implementation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.05.28
 */

#include "assert.h"
#include "ksemaphore.h"
#include "task.h"

void semaphore_pool_init(struct XiziSemaphorePool* sem_pool)
{
    assert(sem_pool != NULL);
    sem_pool->next_sem_id = INVALID_SEM_ID + 1;
    slab_init(&sem_pool->allocator, sizeof(struct ksemaphore), "SemAllocator");
    doubleListNodeInit(&sem_pool->sem_list_guard);
    rbtree_init(&sem_pool->sem_pool_map);
    sem_pool->nr_sem = 0;
}

static inline struct ksemaphore* ksemaphore_get_by_id(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id)
{
    RbtNode* target_sem_node = rbt_search(&sem_pool->sem_pool_map, sem_id);
    if (target_sem_node == NULL) {
        return NULL;
    }
    return (struct ksemaphore*)target_sem_node->data;
}

sem_id_t ksemaphore_alloc(struct XiziSemaphorePool* sem_pool, sem_val_t val)
{
    struct ksemaphore* sem = (struct ksemaphore*)slab_alloc(&sem_pool->allocator);
    if (sem == NULL) {
        ERROR("No memeory to alloc new semaphore.\n");
        return INVALID_SEM_ID;
    }

    /* No error down here */
    /* init ksemaphore since here */
    /// @warning sem->id could overflow
    sem->id = sem_pool->next_sem_id++;
    if (UNLIKELY(sem->id == 0)) {
        slab_free(&sem_pool->allocator, sem);
        return INVALID_SEM_ID;
    }
    sem->val = val;
    doubleListNodeInit(&sem->sem_list_node);
    rbtree_init(&sem->wait_thd_tree);

    if (0 != rbt_insert(&sem_pool->sem_pool_map, sem->id, sem)) {
        slab_free(&sem_pool->allocator, sem);
        return INVALID_SEM_ID;
    }

    doubleListAddOnHead(&sem->sem_list_node, &sem_pool->sem_list_guard);
    sem_pool->nr_sem++;

    return sem->id;
}

bool ksemaphore_consume(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id, sem_val_t decre)
{
    struct ksemaphore* sem = ksemaphore_get_by_id(sem_pool, sem_id);
    // invalid sem id
    if (sem == NULL) {
        return false;
    }

    // if (decre >= 0) {
    sem->val -= decre;
    // }
    return true;
}

bool ksemaphore_wait(struct XiziSemaphorePool* sem_pool, struct Thread* thd, sem_id_t sem_id)
{
    assert(thd != NULL);
    assert(thd->snode.state == RUNNING);
    /* find sem */
    struct ksemaphore* sem = ksemaphore_get_by_id(sem_pool, sem_id);
    // invalid sem id
    if (sem == NULL) {
        return false;
    }
    // DEBUG("%s waiting sem %lu(%d), nr_sem: %d I\n", thd->name, sem_id, sem->val, sem_pool->nr_sem);

    // no need to wait
    if (sem->val > 0) {
        sem->val--;
        return true;
    }

    // waiting at the sem
    sem->val--;
    THREAD_TRANS_STATE(thd, BLOCKED);
    int rbt_insert_res = rbt_insert(&sem->wait_thd_tree, thd->tid, thd);
    assert(RBTTREE_INSERT_SECC == rbt_insert_res || RBTTREE_INSERT_EXISTED == rbt_insert_res);
    return true;
}

bool ksemaphore_signal(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id)
{
    /* find sem */
    struct ksemaphore* sem = ksemaphore_get_by_id(sem_pool, sem_id);
    // invalid sem id
    if (sem == NULL) {
        return false;
    }

    if (sem->val < 0 && !rbt_is_empty(&sem->wait_thd_tree)) {
        assert(!rbt_is_empty(&sem->wait_thd_tree));
        RbtNode* root = sem->wait_thd_tree.root;
        struct Thread* thd = (struct Thread*)root->data;
        rbt_delete(&sem->wait_thd_tree, root->key);
        THREAD_TRANS_STATE(thd, TRANS_WAKING);
    }

    sem->val++;
    return true;
}

bool ksemaphore_signal_no_wake(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id)
{
    /* find sem */
    struct ksemaphore* sem = ksemaphore_get_by_id(sem_pool, sem_id);
    // invalid sem id
    if (sem == NULL) {
        return false;
    }

    sem->val++;
    return true;
}

bool ksemaphore_free(struct XiziSemaphorePool* sem_pool, sem_id_t sem_id)
{
    /* find sem */
    struct ksemaphore* sem = ksemaphore_get_by_id(sem_pool, sem_id);
    // invalid sem id
    if (sem == NULL) {
        return false;
    }

    // by design: no waking any waiting threads

    rbt_delete(&sem_pool->sem_pool_map, sem_id);
    doubleListDel(&sem->sem_list_node);
    slab_free(&sem_pool->allocator, sem);
    sem_pool->nr_sem--;

    return true;
}
