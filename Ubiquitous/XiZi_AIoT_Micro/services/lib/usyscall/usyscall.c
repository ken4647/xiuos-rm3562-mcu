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
#include "usyscall.h"
#include "libmem.h"

uintptr_t sys_test()
{
    return syscall(SYSCALL_TEST, 0, 0, 0, 0);
}

int spawn(struct Session* session, int fd, ipc_read_fn ipc_read, ipc_fsize_fn ipc_fsize, char* name, char** argv)
{
    /* read elf image */
    int max_communicate_size = session->capacity - 0x1000;
    int file_size = ipc_fsize(session, fd);
    void* img = malloc(file_size);
    int read_len = 0;
    while (read_len < file_size) {
        int cur_read_len = file_size - read_len < max_communicate_size ? file_size - read_len : max_communicate_size;
        if (cur_read_len < 0) {
            return -1;
        }
        read_len += ipc_read(session, fd, (char*)((uintptr_t)img + read_len), read_len, cur_read_len);
    }
    /* sys call */
    int ret = syscall(SYSCALL_SPAWN, (uintptr_t)img, (uintptr_t)name, (uintptr_t)argv, 0);
    free(img);
    return ret;
}

int thread(void* entry, const char* name, char** argv)
{
    return syscall(SYSCALL_THREAD, (uintptr_t)entry, (uintptr_t)name, (uintptr_t)argv, 0);
}

void exit(int status)
{
    syscall(SYSCALL_EXIT, (uintptr_t)status, 0, 0, 0);
}

int yield(task_yield_reason reason)
{
    return syscall(SYSCALL_YIELD, (uintptr_t)reason, 0, 0, 0);
}

int kill(int pid)
{
    return syscall(SYSCALL_KILL, (intptr_t)pid, 0, 0, 0);
}

int register_server(char* name)
{
    return syscall(SYSCALL_SERVER, (intptr_t)name, 0, 0, 0);
}

int session(char* path, int capacity, struct Session* user_session)
{
    return syscall(SYSCALL_SESSION, (intptr_t)path, (intptr_t)capacity, (intptr_t)user_session, 0);
}

int poll_session(struct Session* userland_session_arr, int arr_capacity)
{
    return syscall(SYSCALL_POLL_SESSION, (intptr_t)userland_session_arr, (intptr_t)arr_capacity, 0, 0);
}

int close_session(struct Session* session)
{
    return syscall(SYSCALL_CLOSE_SESSION, (intptr_t)session, 0, 0, 0);
}

int wait_session_call(struct Session* userland_session)
{
    return syscall(SYSCALL_WAIT_SESSION, (intptr_t)userland_session, 0, 0, 0);
}

int get_memblock_info(sys_state_info* info)
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_MEMBLOCK_INFO, (intptr_t)info, 0, 0);
}

int set_priority(sys_state_info* info)
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_SET_TASK_PRIORITY, (intptr_t)info, 0, 0);
}

int task_heap_base()
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_GET_HEAP_BASE, 0, 0, 0);
}

int show_task()
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_SHOW_TASKS, 0, 0, 0);
}

int show_mem()
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_SHOW_MEM_INFO, 0, 0, 0);
}

int show_cpu()
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_SHOW_CPU_INFO, 0, 0, 0);
}

int show_actree()
{
    return syscall(SYSCALL_SYS_STATE, SYS_STATE_SHOW_ACTREE, 0, 0, 0);
}

uintptr_t get_second()
{
    sys_state_info info;
    syscall(SYSCALL_SYS_STATE, SYS_STATE_GET_CURRENT_SECOND, (intptr_t)&info, 0, 0);
    return info.current_second;
}

uintptr_t get_tick()
{
    sys_state_info info;
    syscall(SYSCALL_SYS_STATE, SYS_STATE_GET_CURRENT_TICK, (intptr_t)&info, 0, 0);
    return info.current_tick;
}

uintptr_t mmap(uintptr_t vaddr, uintptr_t paddr, int len, bool is_dev)
{
    sys_mmap_info info = {
        .type = SYS_MMAP_NORMAL,
        .is_dev = is_dev,
    };
    uintptr_t vaddr_inner = vaddr, paddr_inner = paddr;
    if (syscall(SYSCALL_MMAP, (intptr_t)&vaddr_inner, (intptr_t)&paddr_inner, (intptr_t)len, (intptr_t)&info) < 0) {
        return (uintptr_t)NULL;
    }
    return vaddr_inner;
}

int naive_mmap(uintptr_t* vaddr, uintptr_t* paddr, int len, bool is_dev)
{
    sys_mmap_info info = {
        .type = SYS_MMAP_NORMAL,
        .is_dev = is_dev,
    };
    return syscall(SYSCALL_MMAP, (uintptr_t)vaddr, (intptr_t)paddr, (intptr_t)len, (intptr_t)&info);
}

int mmap_with_attr(uintptr_t vaddr, uintptr_t paddr, int len, uintptr_t attr)
{
    sys_mmap_info info = {
        .type = SYS_MMAP_CUSTOMIZE,
        .attr = attr,
    };
    return syscall(SYSCALL_MMAP, (intptr_t)vaddr, (intptr_t)paddr, (intptr_t)len, (intptr_t)&info);
}

int register_irq(int irq, int opcode)
{
    return syscall(SYSCALL_REGISTER_IRQ, (intptr_t)irq, (intptr_t)opcode, 0, 0);
}

int semaphore_new(int val)
{
    return syscall(SYSCALL_SEMAPHORE, (intptr_t)SYS_SEM_NEW, (intptr_t)val, 0, 0);
}

bool semaphore_free(int sem_id)
{
    return syscall(SYSCALL_SEMAPHORE, (intptr_t)SYS_SEM_FREE, (intptr_t)sem_id, 0, 0);
}

bool semaphore_wait(int sem_id)
{
    return syscall(SYSCALL_SEMAPHORE, (intptr_t)SYS_SEM_WAIT, (intptr_t)sem_id, 0, 0);
}

bool semaphore_signal(int sem_id)
{
    return syscall(SYSCALL_SEMAPHORE, (intptr_t)SYS_SEM_SIGNAL, (intptr_t)sem_id, 0, 0);
}

int sleep(intptr_t ms)
{
    return syscall(SYSCALL_SLEEP, (intptr_t)ms, 0, 0, 0);
}