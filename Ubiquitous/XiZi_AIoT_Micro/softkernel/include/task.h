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
 * @file task.h
 * @brief task header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: task.h
Description: task header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "core.h"

#include "buddy.h"
#include "ksemaphore.h"
#include "list.h"
#include "memspace.h"
#include "object_allocator.h"
#include "pagetable.h"
#include "queue.h"
#include "share_page.h"
#include "spinlock.h"

#include "scheduler.h"

#define TASK_CLOCK_TICK 50
#define TASK_MAX_PRIORITY 32
#define TASK_DEFAULT_PRIORITY 2
#define TASK_NAME_MAX_LEN 16
#define SLEEP_MONITOR_CORE 0

typedef int tid_t;

/* Thread Control Block */
struct ThreadContext {
    struct Thread* task; // process of current thread

    /* kernel stack of thread */
    uintptr_t kern_stack_addr; // [virt] stack base address

    /* user stack */
    int user_stack_idx; // [virt] stack idx in user memspace
    uintptr_t uspace_stack_addr; // [virt] user stack base address in memspace
    uintptr_t ustack_kvaddr; // [virt] user stack memeory's kernel vaddr

    /* kernel context of thread */
    struct context* context;
    /* user context of thread */
    struct trapframe* trapframe;
};

/* Process Control Block */
struct Thread {
    /* task name */
    char name[TASK_NAME_MAX_LEN];

    /* task debug resources */
    int tid;
    bool bind_irq;
    bool dead;

    /* task context resources */
    struct ThreadContext thread_context; // will only access by task itself

    /* thread mem space */
    struct MemSpace* memspace;
    struct double_list_node memspace_list_node;

    /* task communication resources */
    struct double_list_node cli_sess_listhead;
    struct double_list_node svr_sess_listhead;
    RbtTree cli_sess_map;
    RbtTree svr_sess_map;
    Queue sessions_to_be_handle;
    Queue sessions_in_handle;
    struct TraceTag server_identifier;
    bool advance_unblock; // @todo abandon

    /* task schedule attributes */
    struct ScheduleNode snode;
};

struct SchedulerRightGroup {
    struct TraceTag intr_driver_tag;
    struct TraceTag mmu_driver_tag;
};

/* @todo task pool to maintain task lifetime and support fast task search */
struct GlobalTaskPool {
    RbtTree thd_ref_map;
    struct double_list_node thd_listing_head;
};

struct TaskScheduler {
};

struct TaskLifecycleOperations {
    /* new a task control block, checkout #sys_spawn for usage */
    struct Thread* (*new_thread)(struct MemSpace* pmemspace);
    /* free a task control block, this calls #free_user_pgdir to free all vitual spaces */
    void (*free_thread)(struct Thread*);
};

struct XiziTaskManager {
    TraceTag tag;
    /* thead schedule lists */
    struct double_list_node task_list_head[TASK_MAX_PRIORITY]; /* list of task control blocks that are allocated */
    struct double_list_node task_running_list_head;
    struct double_list_node task_blocked_list_head;
    struct double_list_node task_sleep_list_head;
    struct XiziSemaphorePool semaphore_pool;
    /* living task pool */
    TraceTag task_pool_tag;
    /* task lifecycle Ops */
    TraceTag task_lifecycle_ops_tag;

    /* mem allocator */
    struct slab_allocator memspace_allocator;
    struct slab_allocator task_allocator; // allocate struct Tread
    struct slab_allocator task_buddy_allocator; // allocate buddy for memspace
    uint32_t next_pid;

    /* init task manager */
    void (*init)();
    /* use by task_scheduler, find next READY task, should be in locked */
    struct Thread* (*next_runnable_task)(void);
    /* function that's runing by kernel thread context, schedule use tasks */
    void (*task_scheduler)(struct SchedulerRightGroup);

    /* handle task state */
    /* call to yield current use task */
    void (*task_yield_noschedule)(struct Thread* task, bool is_blocking);
    /* set task priority */
    void (*set_cur_task_priority)(int priority);
};

extern uint32_t ready_task_priority;
extern struct Thread* next_task_emergency;
extern struct XiziTaskManager xizi_task_manager;

bool module_task_manager_init(TraceTag* softkernel_tag);
