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
 * @file share_page.c
 * @brief support share page for tasks
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: share_page.c
Description: support share page for tasks
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "cache_common_ope.h"

#include "assert.h"
#include "kalloc.h"
#include "multicores.h"
#include "object_allocator.h"
#include "pagetable.h"
#include "share_page.h"
#include "task.h"

static struct SharePageRightGroup right_group;

static struct slab_allocator* SessionAllocator()
{
    static bool init = false;
    static struct slab_allocator session_slab;
    if (!init) {
        slab_init(&session_slab, sizeof(struct session_backend), "SessionAllocator");
    }
    return &session_slab;
}

/// @brief  check if [vaddr, vaddr + nr_pages * PAGE_SIZE) is mapped in task's vspace
/// @param task
/// @param vaddr
/// @param nr_pages
/// @return true if mem range is free, false if at least one page inside [vaddr, vaddr + nr_pages * PAGE_SIZE) is mapped
static inline bool check_pages_unmapped(struct Thread* task, uintptr_t vaddr, int nr_pages)
{
    for (uintptr_t i = 0; i < nr_pages; i++) {
        uintptr_t paddr = UINT32_MAX;
        if ((paddr = xizi_pager.address_translate(&task->memspace->pgdir, vaddr)) != (uintptr_t)NULL) {
            return false;
        }
        vaddr += PAGE_SIZE;
    }
    return true;
}

/// @brief get a addr to map ipc page
/// @param task
/// @param nr_pages continuously map nr_pages
/// @return addr to be mapped, aligned by page
/// @todo optimize, and suppurt multiple pages
static uintptr_t alloc_share_page_addr(struct Thread* task, const int nr_pages)
{
    uintptr_t vaddr = USER_IPC_SPACE_BASE;
    while (!check_pages_unmapped(task, vaddr, nr_pages)) {
        // vaddr is destinate to be (2 * PAGE_SIZE) aligned
        vaddr += 2 * PAGE_SIZE;
        assert(vaddr % PAGE_SIZE == 0);
    }

    // now that nr_pages size after vaddr is unmapped
    if (UNLIKELY(vaddr >= USER_IPC_SPACE_TOP)) {
        return (uintptr_t)NULL;
    }

    return vaddr;
}

static uintptr_t map_task_share_page(struct Thread* task, const uintptr_t paddr, const int nr_pages)
{
    /* get driver codes */
    struct DCacheDone* p_dcache_done = AchieveResource(&right_group.dcache_driver_tag);
    struct MmuCommonDone* p_mmu_driver = AchieveResource(&right_group.mmu_driver_tag);

    // map double vaddr page to support uniform ring buffer r/w
    uintptr_t vaddr = (uintptr_t)NULL;
    if (task->memspace->massive_ipc_allocator != NULL) {
        // alloc from ipc area buddy
        vaddr = (uintptr_t)KBuddyAlloc(task->memspace->massive_ipc_allocator, PAGE_SIZE * nr_pages * 2);
        if (vaddr == (uintptr_t)NULL) {
            ERROR("Task %s drains all ipc area.\n", task->name);
            return (uintptr_t)NULL;
        }

        // allocated ipc share vaddr must not been used
        assert(xizi_pager.address_translate(&task->memspace->pgdir, vaddr) == (uintptr_t)NULL);
    } else {
        // simple allocation
        vaddr = alloc_share_page_addr(task, nr_pages * 2);

        // time to use buddy
        if (vaddr + (2 * nr_pages * PAGE_SIZE) >= USER_IPC_USE_ALLOCATOR_WATERMARK) {
            task->memspace->massive_ipc_allocator = (struct KBuddy*)slab_alloc(&xizi_task_manager.task_buddy_allocator);
            if (!task->memspace->massive_ipc_allocator) {
                ERROR("Alloc task buddy failed.\n");
                return (uintptr_t)NULL;
            }
            if (!KBuddyInit(task->memspace->massive_ipc_allocator, USER_IPC_USE_ALLOCATOR_WATERMARK, USER_IPC_SPACE_TOP)) {
                ERROR("Alloc task buddy failed.\n");
                slab_free(&xizi_task_manager.task_buddy_allocator, task->memspace->massive_ipc_allocator);
                task->memspace->massive_ipc_allocator = NULL;
                return (uintptr_t)NULL;
            }

            return map_task_share_page(task, paddr, nr_pages);
        }
    }

    if (UNLIKELY(vaddr == (uintptr_t)NULL)) {
        return (uintptr_t)NULL;
    }

    // map first area
    if (!xizi_pager.map_pages(task->memspace, vaddr, paddr, nr_pages * PAGE_SIZE, false)) {
        return (uintptr_t)NULL;
    }

    // map second area
    if (!xizi_pager.map_pages(task->memspace, vaddr + (nr_pages * PAGE_SIZE), paddr, nr_pages * PAGE_SIZE, false)) {
        xizi_pager.unmap_pages(task->memspace->pgdir.pd_addr, vaddr, nr_pages * PAGE_SIZE);
        return (uintptr_t)NULL;
    }

    // flush tlb
    if (task == cur_cpu()->task) {
        p_mmu_driver->TlbFlush(vaddr, 2 * nr_pages * PAGE_SIZE);
        /// @todo clean range rather than all
        p_dcache_done->invalidateall();
    }

    return vaddr;
}

uintptr_t task_map_pages(struct Thread* task, const uintptr_t vaddr, const uintptr_t paddr, const int nr_pages, const int is_dev)
{
    /* get driver codes */
    struct DCacheDone* p_dcache_done = AchieveResource(&right_group.dcache_driver_tag);
    struct MmuCommonDone* p_mmu_driver = AchieveResource(&right_group.mmu_driver_tag);

    bool ret = false;
    if (is_dev) {
        ret = xizi_pager.map_pages(task->memspace, vaddr, paddr, nr_pages * PAGE_SIZE, true);
    } else {
        ret = xizi_pager.map_pages(task->memspace, vaddr, paddr, nr_pages * PAGE_SIZE, false);
    }

    if (!ret) {
        return (uintptr_t)NULL;
    }

    if (task == cur_cpu()->task) {
        p_mmu_driver->TlbFlush(vaddr, nr_pages * PAGE_SIZE);
        /// @todo clean range rather than all
        p_dcache_done->invalidateall();
    }

    return vaddr;
}

void unmap_task_share_pages(struct Thread* task, const uintptr_t task_vaddr, const int nr_pages)
{
    // usages of unmap_task_share_pages must be correct
    assert(task_vaddr >= USER_IPC_SPACE_BASE && task_vaddr < USER_IPC_SPACE_TOP);

    /* get driver codes */
    struct DCacheDone* p_dcache_done = AchieveResource(&right_group.dcache_driver_tag);
    struct MmuCommonDone* p_mmu_driver = AchieveResource(&right_group.mmu_driver_tag);

    // unmap must be correct
    assert(xizi_pager.unmap_pages(task->memspace->pgdir.pd_addr, task_vaddr, nr_pages * PAGE_SIZE));
    assert(xizi_pager.unmap_pages(task->memspace->pgdir.pd_addr, task_vaddr + (nr_pages * PAGE_SIZE), nr_pages * PAGE_SIZE));

    // retrieve virtual address
    if (task_vaddr >= USER_IPC_USE_ALLOCATOR_WATERMARK) {
        KBuddyFree(task->memspace->massive_ipc_allocator, (void*)task_vaddr);
    }

    if (task == cur_cpu()->task) {
        p_mmu_driver->TlbFlush(task_vaddr, 2 * nr_pages * PAGE_SIZE);
        /// @todo clean range rather than all
        p_dcache_done->invalidateall();
    }
}

static int next_session_id = 1;
struct session_backend* create_share_pages(struct Thread* client, struct Thread* server, const int capacity)
{

    /* alloc session backend */
    struct session_backend* session_backend = (struct session_backend*)slab_alloc(SessionAllocator());
    if (UNLIKELY(session_backend == NULL)) {
        return NULL;
    }
    session_backend->session_id = next_session_id++;

    if (0 != rbt_insert(&client->cli_sess_map, session_backend->session_id, &session_backend->client_side)) {
        DEBUG("Rbt of %s no memory\n", client->name);
        slab_free(SessionAllocator(), session_backend);
        return NULL;
    }

    if (0 != rbt_insert(&server->svr_sess_map, session_backend->session_id, &session_backend->server_side)) {
        DEBUG("Rbt of %s no memory\n", server->name);
        rbt_delete(&client->cli_sess_map, session_backend->session_id);
        slab_free(SessionAllocator(), session_backend);
        return NULL;
    }

    sem_id_t new_sem_id = ksemaphore_alloc(&xizi_task_manager.semaphore_pool, 0);
    if (new_sem_id == INVALID_SEM_ID) {
        ERROR("No memory to alloc sem\n");
        slab_free(SessionAllocator(), session_backend);
        return NULL;
    }
    session_backend->client_sem_to_wait = new_sem_id;

    int true_capacity = ALIGNUP(capacity, PAGE_SIZE);
    int nr_pages = true_capacity / PAGE_SIZE;
    /* alloc free memory as share memory */
    uintptr_t kern_vaddr = (uintptr_t)kalloc(true_capacity);
    if (UNLIKELY(kern_vaddr == (uintptr_t)NULL)) {
        ERROR("No memory for session\n");
        slab_free(SessionAllocator(), session_backend);
        ksemaphore_free(&xizi_task_manager.semaphore_pool, new_sem_id);
        return NULL;
    }

    assert(kern_vaddr % PAGE_SIZE == 0);
    /* map client vspace */
    uintptr_t client_vaddr = map_task_share_page(client, V2P_WO(kern_vaddr), nr_pages);
    if (UNLIKELY(client_vaddr == (uintptr_t)NULL)) {
        kfree((char*)kern_vaddr);
        slab_free(SessionAllocator(), session_backend);
        ksemaphore_free(&xizi_task_manager.semaphore_pool, new_sem_id);
        return NULL;
    }

    /* map server vspace */
    uintptr_t server_vaddr = map_task_share_page(server, V2P_WO(kern_vaddr), nr_pages);
    if (UNLIKELY(server_vaddr == (uintptr_t)NULL)) {
        unmap_task_share_pages(client, client_vaddr, nr_pages);
        kfree((char*)kern_vaddr);
        slab_free(SessionAllocator(), session_backend);
        ksemaphore_free(&xizi_task_manager.semaphore_pool, new_sem_id);
        return NULL;
    }

    /* build session_backend */
    session_backend->buf_kernel_addr = kern_vaddr;
    session_backend->nr_pages = nr_pages;
    session_backend->client = client;
    session_backend->server = server;
    // init client side session struct
    session_backend->client_side.buf_addr = client_vaddr;
    session_backend->client_side.capacity = true_capacity;
    session_backend->client_side.closed = false;
    doubleListNodeInit(&session_backend->client_side.node);
    doubleListAddOnBack(&session_backend->client_side.node, &client->cli_sess_listhead);
    // init server side session struct
    session_backend->server_side.buf_addr = server_vaddr;
    session_backend->server_side.capacity = true_capacity;
    session_backend->server_side.head = 0;
    session_backend->server_side.tail = 0;
    session_backend->server_side.closed = false;
    doubleListNodeInit(&session_backend->server_side.node);
    doubleListAddOnBack(&session_backend->server_side.node, &server->svr_sess_listhead);

    server->memspace->mem_size += true_capacity;
    client->memspace->mem_size += true_capacity;

    return session_backend;
}

/**
 * @brief deletion of a session happens at 3 conditions:
 *  1. task exits, close its side of sessions and found opposite side is also closed.
 *  2. server poll found a closed client side session.
 *  3. client close a session, and found server side has closed.
 */
int delete_share_pages(struct session_backend* session_backend)
{
    if (session_backend == NULL) {
        return -1;
    }

    assert(session_backend->server_side.closed || session_backend->client_side.closed);

    /* unmap share pages */
    // close ssesion in server's perspective
    if (session_backend->server_side.closed && session_backend->server != NULL) {
        xizi_share_page_manager.unmap_task_share_pages(session_backend->server, session_backend->server_side.buf_addr, session_backend->nr_pages);

        ERROR_FREE
        {
            assert(0 == rbt_delete(&session_backend->server->svr_sess_map, session_backend->session_id));
            doubleListDel(&session_backend->server_side.node);
            session_backend->server->memspace->mem_size -= session_backend->nr_pages * PAGE_SIZE;
            session_backend->server = NULL;
        }
    }

    // close ssesion in client's perspective
    if (session_backend->client_side.closed && session_backend->client != NULL) {
        xizi_share_page_manager.unmap_task_share_pages(session_backend->client, session_backend->client_side.buf_addr, session_backend->nr_pages);

        ERROR_FREE
        {
            assert(0 == rbt_delete(&session_backend->client->cli_sess_map, session_backend->session_id));
            doubleListDel(&session_backend->client_side.node);
            session_backend->client->memspace->mem_size -= session_backend->nr_pages * PAGE_SIZE;
            session_backend->client = NULL;

            assert(ksemaphore_free(&xizi_task_manager.semaphore_pool, session_backend->client_sem_to_wait));
        }
    }

    /* free seesion backend */
    if (session_backend->server_side.closed && session_backend->client_side.closed) {
        ERROR_FREE
        {
            assert(session_backend->client == NULL && session_backend->server == NULL);
            assert(kfree((void*)session_backend->buf_kernel_addr));
            slab_free(SessionAllocator(), (void*)session_backend);
        }
    }

    return 0;
}

struct XiziSharePageManager xizi_share_page_manager = {
    .create_share_pages = create_share_pages,
    .unmap_task_share_pages = unmap_task_share_pages,
    .delete_share_pages = delete_share_pages,
    .task_map_pages = task_map_pages,
};

int module_share_page_init(struct SharePageRightGroup* _right_group)
{
    /* assign rights to share page module */
    right_group = *_right_group;
    return 0;
}