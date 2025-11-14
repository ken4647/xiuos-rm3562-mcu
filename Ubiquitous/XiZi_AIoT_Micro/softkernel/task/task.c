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
 * @file task.c
 * @brief task implementation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: task.c
Description: task implementation
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <string.h>

#include "core.h"

#include "assert.h"
#include "kalloc.h"
#include "memspace.h"
#include "multicores.h"
#include "schedule_algo.h"
#include "syscall.h"
#include "task.h"
#include "trap_common.h"

struct CPU global_cpus[NR_CPU];
uint32_t ready_task_priority;

struct GlobalTaskPool global_task_pool;
struct Scheduler g_scheduler;
extern struct TaskLifecycleOperations task_lifecycle_ops;

static void _task_manager_init()
{
    assert(CreateResourceTag(&xizi_task_manager.task_lifecycle_ops_tag, &xizi_task_manager.tag, //
        "TaskLifeCycleOpTool", TRACER_SYSOBJECT, (void*)&task_lifecycle_ops));

    // init task list to NULL
    for (int i = 0; i < TASK_MAX_PRIORITY; i++) {
        doubleListNodeInit(&xizi_task_manager.task_list_head[i]);
    }

    /* task scheduling list */
    doubleListNodeInit(&xizi_task_manager.task_blocked_list_head);
    doubleListNodeInit(&xizi_task_manager.task_running_list_head);
    doubleListNodeInit(&xizi_task_manager.task_sleep_list_head);

    // init task (slab) allocator
    slab_init(&xizi_task_manager.memspace_allocator, sizeof(struct MemSpace), "MemlpaceCtrlBlockAllocator");
    slab_init(&xizi_task_manager.task_allocator, sizeof(struct Thread), "TreadCtrlBlockAllocator");
    slab_init(&xizi_task_manager.task_buddy_allocator, sizeof(struct KBuddy), "DMBuddyAllocator");

    /* global semaphore factory */
    semaphore_pool_init(&xizi_task_manager.semaphore_pool);

    /* task pool */
    doubleListNodeInit(&global_task_pool.thd_listing_head);
    rbtree_init(&global_task_pool.thd_ref_map);

    // scheduler
    assert(CreateResourceTag(&g_scheduler.tag, &xizi_task_manager.tag, //
        "GlobalScheduler", TRACER_SYSOBJECT, (void*)&g_scheduler));
    semaphore_pool_init(&g_scheduler.semaphore_pool);
    for (int pool_id = 0; pool_id < NR_STATE; pool_id++) {
        rbtree_init(&g_scheduler.snode_state_pool[pool_id]);
    }
    rbtree_init(&g_scheduler.state_trans_ref_map);

    // tid pool
    xizi_task_manager.next_pid = 1;

    // init priority bit map
    ready_task_priority = 0;
}

int _task_return_sys_resources(struct Thread* ptask)
{
    assert(ptask != NULL);

    /* handle sessions for condition 1, ref. delete_share_pages() */
    // close all server_sessions
    while (!IS_DOUBLE_LIST_EMPTY(&ptask->svr_sess_listhead)) {
        // RbtNode* sess_ref_node = ptask->svr_sess_map.root;
        struct server_session* svr_session = CONTAINER_OF(ptask->svr_sess_listhead.next, struct server_session, node);
        server_close_session(ptask, svr_session);
    }

    // close all client_sessions
    while (!IS_DOUBLE_LIST_EMPTY(&ptask->cli_sess_listhead)) {
        // RbtNode* sess_ref_node = ptask->cli_sess_map.root;
        struct client_session* cli_session = CONTAINER_OF(ptask->cli_sess_listhead.next, struct client_session, node);
        client_close_session(ptask, cli_session);

        // info server that session is closed
        struct session_backend* session_backend = CLIENT_SESSION_BACKEND(cli_session);
        struct Thread* server_to_info = session_backend->server;
        if (!enqueue(&server_to_info->sessions_to_be_handle, 0, (void*)&session_backend->server_side)) {
            // @todo fix memory leak
        } else {
            assert(!queue_is_empty(&server_to_info->sessions_to_be_handle));
            THREAD_TRANS_STATE(server_to_info, BLOCKED);
        }
    }

    /* delete server identifier */
    if (ptask->server_identifier.meta != NULL) {
        // @todo figure out server-identifier ownership
        struct TraceTag server_identifier_owner;
        AchieveResourceTag(&server_identifier_owner, RequireRootTag(), "softkernel/server-identifier");
        assert(server_identifier_owner.meta != NULL);
        assert(DeleteResource(&ptask->server_identifier, &server_identifier_owner));
    }

    // delete registered irq if there is one
    if (ptask->bind_irq) {
        sys_unbind_irq_all(ptask);
    }

    return 0;
}

#ifndef __riscv
extern void trap_return(void);
__attribute__((optimize("O0"))) void task_prepare_enter()
{
    xizi_leave_kernel();
    trap_return();
}
#endif

/// @brief this function changes task list without locking, so it must be called inside a lock critical area
/// @param task
static void _free_thread(struct Thread* task)
{
    if (UNLIKELY(task == NULL)) {
        ERROR("deallocating a NULL task\n");
        return;
    }

    _task_return_sys_resources(task);

    /* free thread's user stack */
    if (task->thread_context.user_stack_idx != -1) {
        // stack is mapped in vspace, so it should be freed from pgdir
        assert(task->thread_context.user_stack_idx >= 0 && task->thread_context.user_stack_idx < 64);
        assert(task->memspace != NULL);

        /* the stack must have be set in memspace if bitmap has been set */
        assert(xizi_pager.unmap_pages(task->memspace->pgdir.pd_addr, task->thread_context.uspace_stack_addr, USER_STACK_SIZE));
        bitmap64_free(&task->memspace->thread_stack_idx_bitmap, task->thread_context.user_stack_idx);
        /* thread's user stack space is also allocated for kernel free space */
        assert(kfree((char*)task->thread_context.ustack_kvaddr));

        if (task->memspace != NULL) {
            task->memspace->mem_size -= USER_STACK_SIZE;
        }
    }

    // remove thread from used task list
    task_dead(task);

    /* free memspace if needed to */
    if (task->memspace != NULL) {
        /* free thread's kernel stack */
        if (task->thread_context.kern_stack_addr) {
            // kfree_by_ownership(task->memspace->kernspace_mem_usage.tag, (char*)task->thread_context.kern_stack_addr);
        }

        // awake deamon in this memspace
        if (task->memspace->thread_to_notify != NULL) {
            if (task->memspace->thread_to_notify != task) {
                if (task->memspace->thread_to_notify->snode.state == BLOCKED) {
                    THREAD_TRANS_STATE(task->memspace->thread_to_notify, TRANS_WAKING);
                } else {
                    task->memspace->thread_to_notify->advance_unblock = true;
                }
            } else if (task->memspace->thread_to_notify == task) {
                task->memspace->thread_to_notify = NULL;
            }
        }

        doubleListDel(&task->memspace_list_node);

        /* free memspace if thread is the last one using it */
        if (IS_DOUBLE_LIST_EMPTY(&task->memspace->thread_list_guard)) {
            // free memspace
            free_memspace(task->memspace);
        }
    }

    // free task back to allocator
    slab_free(&xizi_task_manager.task_allocator, (void*)task);
}

/* alloc a new task with init */
static struct Thread* _new_thread(struct MemSpace* pmemspace)
{
    assert(pmemspace != NULL);

    //  alloc task space
    struct Thread* task = (struct Thread*)slab_alloc(&xizi_task_manager.task_allocator);
    if (task == NULL) {
        ERROR("Not enough memory\n");
        return NULL;
    }

    // [schedule related]
    task->tid = xizi_task_manager.next_pid++;
    if (!init_schedule_node(&task->snode, task)) {
        ERROR("Not enough memory\n");
        slab_free(&xizi_task_manager.task_allocator, (void*)task);
        return NULL;
    }

    // alloc stack page for task
    if ((void*)(task->thread_context.kern_stack_addr = (uintptr_t)kalloc_by_ownership(pmemspace->kernspace_mem_usage.tag, USER_STACK_SIZE)) == NULL) {
        /* here inside, will no free memspace */
        assert(RBTTREE_DELETE_SUCC == rbt_delete(&g_scheduler.snode_state_pool[INIT], task->snode.snode_id));
        slab_free(&xizi_task_manager.task_allocator, (void*)task);
        return NULL;
    }

    ERROR_FREE
    {
        /* init basic task ref member */
        task->bind_irq = false;

        /* vm & memory member */
        task->thread_context.user_stack_idx = -1;
        task->memspace = pmemspace;
        doubleListNodeInit(&task->memspace_list_node);
        doubleListAddOnBack(&task->memspace_list_node, &pmemspace->thread_list_guard);

        /* thread context */
        task->thread_context.task = task;
        memset((void*)task->thread_context.kern_stack_addr, 0x00, USER_STACK_SIZE);
        /// stack bottom
#ifndef __riscv
        char* sp = (char*)task->thread_context.kern_stack_addr + USER_STACK_SIZE - 4;
#else
        char* sp = (char*)task->thread_context.kern_stack_addr + USER_STACK_SIZE;
#endif

        /// 1. trap frame into stack, for process to nomally return by trap_return
        /// trapframe (user context)
        sp -= sizeof(*task->thread_context.trapframe);
        task->thread_context.trapframe = (struct trapframe*)sp;

        /// 2. context into stack
        // (kernel context)
        sp -= sizeof(*task->thread_context.context);
        task->thread_context.context = (struct context*)sp;
        arch_init_context(task->thread_context.context);

        /* ipc member */
        doubleListNodeInit(&task->cli_sess_listhead);
        doubleListNodeInit(&task->svr_sess_listhead);
        rbtree_init(&task->cli_sess_map);
        rbtree_init(&task->svr_sess_map);
        queue_init(&task->sessions_in_handle);
        queue_init(&task->sessions_to_be_handle);
        /// server identifier
        task->server_identifier.meta = NULL;
    }

    // [name]

    return task;
}

struct TaskLifecycleOperations task_lifecycle_ops = {
    .new_thread = _new_thread,
    .free_thread = _free_thread,
};

static void task_state_set_running(struct Thread* task)
{
    assert(task != NULL && task->snode.state == READY);
    assert(task_trans_sched_state(&task->snode, //
        &g_scheduler.snode_state_pool[READY], //
        &g_scheduler.snode_state_pool[RUNNING], RUNNING));
}

bool rbt_in_queue(RbtNode* node, void* data)
{
    Queue* queue = (Queue*)data;
    return enqueue(queue, node->key, node->data);
}

extern void show_tasks(void);
static void central_trans_task_state()
{
    Queue tmp_queue;
    queue_init(&tmp_queue);
    rbt_traverse(&g_scheduler.state_trans_ref_map, rbt_in_queue, (void*)&tmp_queue);

    while (!queue_is_empty(&tmp_queue)) {
        struct Thread* thd = (struct Thread*)queue_front(&tmp_queue)->data;
        struct ScheduleNode* snode = &thd->snode;
        assert(cur_cpu()->task != NULL);
        if (snode->state == RUNNING && cur_cpu()->task->tid != thd->tid) {
            dequeue(&tmp_queue);
            continue;
        }

        Queue* trans_queue = &snode->state_trans_signal_queue;
        while (!queue_is_empty(trans_queue)) {
            QueueNode* cur_qnode = queue_front(trans_queue);
            enum ThreadState next_state = cur_qnode->key;
            switch (next_state) {
            case READY: {
                if (snode->state == RUNNING || snode->state == READY) {
                    task_into_ready(thd);
                } else {
                    ERROR("Thread %s(%d) Error trans to READY(from %d)\n", thd->name, thd->tid, snode->state);
                }
                break;
            }
            case BLOCKED: {
                if (snode->sched_context.unblock_signals > 0) {
                    snode->sched_context.unblock_signals--;
                    task_into_ready(thd);
                } else {
                    task_block(thd);
                }
                break;
            }
            case SLEEPING: {
                /// @todo support sleep
                break;
            }
            case TRANS_WAKING: {
                if (snode->state == BLOCKED) {
                    task_into_ready(thd);
                } else {
                    snode->sched_context.unblock_signals++;
                    task_into_ready(thd);
                }
                break;
            }
            case DEAD: {
                /// @todo
                break;
            }
            default:
                break;
            }

            dequeue(trans_queue);
        }

        assert(RBTTREE_DELETE_SUCC == rbt_delete(&g_scheduler.state_trans_ref_map, thd->tid));
        dequeue(&tmp_queue);
    }
}

#ifdef __riscv
uintptr_t riscv_kernel_satp = 0;
#endif
struct Thread* next_task_emergency = NULL;
#ifndef __riscv
extern void context_switch(struct context**, struct context*);
#else
extern void context_switch(struct context*, struct context*);
#endif
static void _scheduler(struct SchedulerRightGroup right_group)
{
    struct MmuCommonDone* p_mmu_driver = AchieveResource(&right_group.mmu_driver_tag);
    struct Thread* next_task;
    struct CPU* cpu = cur_cpu();

    while (1) {
        next_task = NULL;
        /* find next runnable task */
        assert(cur_cpu()->task == NULL);
        if (next_task_emergency != NULL && next_task_emergency->snode.state == READY) {
            next_task = next_task_emergency;
        } else {
            next_task = xizi_task_manager.next_runnable_task();
        }
        next_task_emergency = NULL;

        /* if there's not a runnable task, wait for one */
        if (next_task == NULL) {
            xizi_leave_kernel();
            /* leave kernel for other cores, so they may create a runnable task */
            xizi_enter_kernel();
            continue;
        }

        /* run the chosen task */
        // DEBUG_PRINTF("Thread %s(%d) to RUNNING\n", next_task->name, next_task->tid);
        task_state_set_running(next_task);
        cpu->task = next_task;

#ifdef __riscv
        riscv_kernel_satp = PFN_DOWN((uintptr_t)V2P(next_task->memspace->pgdir_riscv.pd_addr)) | SATP_MODE;
#endif

        assert(next_task->memspace->pgdir.pd_addr != NULL);
        p_mmu_driver->LoadPgdir((uintptr_t)V2P(next_task->memspace->pgdir.pd_addr));

        context_switch(&cpu->scheduler, next_task->thread_context.context);
        central_trans_task_state();
        cpu->task = NULL;
    }
}

/// @brief  @warning not tested function
/// @param priority
static void _set_cur_task_priority(int priority)
{
    return;
}

struct XiziTaskManager xizi_task_manager = {
    .init = _task_manager_init,
    .next_runnable_task = max_priority_runnable_task,
    .task_scheduler = _scheduler,
    .set_cur_task_priority = _set_cur_task_priority
};

bool module_task_manager_init(TraceTag* softkernel_tag)
{
    CreateResourceTag(&xizi_task_manager.tag, softkernel_tag, "KTaskManager", TRACER_OWNER, &xizi_task_manager);
    xizi_task_manager.init();
    return true;
}
