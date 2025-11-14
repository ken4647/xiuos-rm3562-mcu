
#pragma once
#include "actracer.h"
#include "ksemaphore.h"
#include "rbtree.h"

#define TASK_MAX_PRIORITY 32
#define UNINIT_SNODE_ID 0
typedef uintptr_t snode_id_t;

enum ThreadState {
    NEVER_RUN = 0,
    INIT,
    READY,
    RUNNING,
    DEAD,
    BLOCKED,
    SLEEPING,
    NR_STATE,

    // follow state is temp for kernel use
    TRANS_WAKING,
};

typedef struct ScheduleContext {
    intptr_t remain_tick;
    uint64_t run_time;
    intptr_t unblock_signals;
} ScheduleContext;

typedef struct TaskSleepContext {
    int64_t remain_ms;
} TaskSleepContext;

struct ScheduleNode {
    struct Thread* pthd;
    snode_id_t snode_id;
    enum ThreadState state;
    Queue state_trans_signal_queue;

    ScheduleContext sched_context;
    TaskSleepContext sleep_context;
};

struct Scheduler {
    TraceTag tag;
    RbtTree snode_state_pool[NR_STATE];
    RbtTree state_trans_ref_map;
    struct XiziSemaphorePool semaphore_pool;
};

extern struct Scheduler g_scheduler;

bool init_schedule_node(struct ScheduleNode* snode, struct Thread* bind_thd);
void enqueue_task_trans_state(struct Thread* thd, enum ThreadState state);
#define THREAD_TRANS_STATE(thd, state) enqueue_task_trans_state(thd, state);

bool task_trans_sched_state(struct ScheduleNode* snode, RbtTree* from_pool, RbtTree* to_pool, enum ThreadState target_state);
void task_block(struct Thread* thd);
void task_dead(struct Thread* thd);
void task_into_ready(struct Thread* thd);
