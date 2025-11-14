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
#pragma once
#include <stdint.h>

#include "libserial.h"
#include "session.h"

// clang-format off
#define SYSCALL_TEST            0
#define SYSCALL_SPAWN           1   // generate a brand new task to run elf
#define SYSCALL_EXIT            2   // exit task, delete the task cb
#define SYSCALL_YIELD           3   // yield task, go to scheduler

#define SYSCALL_MMAP            4   // map a virt page to phy page
#define SYSCALL_SERVER          5   // register current task as a server
#define SYSCALL_SESSION         6   // create a session to a server
#define SYSCALL_POLL_SESSION    7   // server poll for it's server sessions
#define SYSCALL_CLOSE_SESSION   8   // client close it's client sessions

#define SYSCALL_THREAD          9   // generate a thread using old memspace
#define SYSCALL_SYS_STATE       10  // run system state
#define SYSCALL_REGISTER_IRQ    11  //

#define SYSCALL_KILL            12  // kill the task by id

#define SYSCALL_SEMAPHORE       13  // semaphore related operations
#define SYSCALL_SLEEP           14  // sleep

#define SYSCALL_WAIT_SESSION    15
// clang-format on

typedef enum {
    SYS_STATE_TEST = 0,
    SYS_STATE_SET_TASK_PRIORITY,
    SYS_STATE_GET_HEAP_BASE,
    SYS_STATE_MEMBLOCK_INFO,
    SYS_STATE_SHOW_TASKS,
    SYS_STATE_SHOW_MEM_INFO,
    SYS_STATE_SHOW_CPU_INFO,
    SYS_STATE_GET_CURRENT_TICK,
    SYS_STATE_GET_CURRENT_SECOND,
    SYS_STATE_SHOW_ACTREE,
} sys_state_option;

typedef enum {
    SYS_TASK_YIELD_NO_REASON = 0x0,
    SYS_TASK_YIELD_FOREVER = 0x1,
    SYS_TASK_YIELD_BLOCK_IPC = 0x2,
} task_yield_reason;

typedef enum {
    SYS_MMAP_NORMAL = 0x0,
    SYS_MMAP_CUSTOMIZE,
} sys_mmap_type;

typedef struct sys_mmap_info {
    sys_mmap_type type;
    uintptr_t attr;
    bool is_dev;
} sys_mmap_info;

typedef union {
    struct {
        uintptr_t memblock_start;
        uintptr_t memblock_end;
    } memblock_info;
    int priority;
    uintptr_t current_tick;
    uintptr_t current_second;
} sys_state_info;

typedef enum {
    SYS_SEM_NEW = 0,
    SYS_SEM_FREE,
    SYS_SEM_SIGNAL,
    SYS_SEM_WAIT,
} sys_sem_option;

typedef int (*ipc_read_fn)(struct Session* session, int fd, char* dst, int offset, int len);
typedef int (*ipc_fsize_fn)(struct Session* session, int fd);
typedef int (*ipc_write_fn)(struct Session* session, int fd, char* src, int offset, int len);

int syscall(int sys_num, intptr_t a1, intptr_t a2, intptr_t a3, intptr_t a4);
uintptr_t syscall_ori(int sys_num, intptr_t a1, intptr_t a2, intptr_t a3, intptr_t a4);

uintptr_t sys_test();

int spawn(struct Session* session, int fd, ipc_read_fn ipc_read, ipc_fsize_fn ipc_fsize, char* name, char** argv);
int thread(void* entry, const char* name, char** argv);
void exit(int status);
int yield(task_yield_reason reason);
int kill(int pid);

int register_server(char* name);
int session(char* path, int capacity, struct Session* user_session);
int poll_session(struct Session* userland_session, int arr_capacity);
int wait_session_call(struct Session* userland_session);
int close_session(struct Session* session);
int register_irq(int irq, int opcode);

uintptr_t mmap(uintptr_t vaddr, uintptr_t paddr, int len, bool is_dev);
int naive_mmap(uintptr_t* vaddr, uintptr_t* paddr, int len, bool is_dev);
int customized_mmap(uintptr_t vaddr, uintptr_t paddr, int len, uintptr_t attr);

int task_heap_base();
int get_memblock_info(sys_state_info* info);
int set_priority(sys_state_info* info);
int show_task();
int show_mem();
int show_cpu();
int show_actree();

uintptr_t get_second();
uintptr_t get_tick();

int semaphore_new(int val);
bool semaphore_free(int sem_id);
bool semaphore_wait(int sem_id);
bool semaphore_signal(int sem_id);

int sleep(intptr_t ms);