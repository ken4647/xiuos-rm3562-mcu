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
 * @file memspace.h
 * @brief memspace loader
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: memspace.h
Description: memspace loader
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "actracer.h"
#include "bitmap64.h"
#include "buddy.h"
#include "kalloc.h"
#include "list.h"

struct TopLevelPageDirectory {
    uintptr_t* pd_addr;
};

struct ThreadStackPointer {
    int argc;
    int stack_idx;
    uintptr_t user_sp;
    uintptr_t user_stack_vaddr;
};

struct MemSpace {
    /* trace node */
    TraceTag tag;
    /* mem usage info */
    struct MemUsage kernspace_mem_usage;
    struct MemUsage userspace_mem_usage;
    struct MemUsage customized_mapping_mem_map;

    /* task memory resources */
    struct TopLevelPageDirectory pgdir; // [phy] vm pgtbl base address
    struct TopLevelPageDirectory pgdir_riscv; // [phy] vm pgtbl base address
    uintptr_t heap_base; // mem size of proc used(allocated by kernel)
    uintptr_t mem_size;
    /* task communication mem resources */
    struct KBuddy* massive_ipc_allocator;

    /* thread using this memspace */
    struct bitmap64 thread_stack_idx_bitmap;
    struct double_list_node thread_list_guard;

    // thread to notify when sub-thread exit
    struct Thread* thread_to_notify;
};

struct MemSpace* alloc_memspace(char* name);
void free_memspace(struct MemSpace* pmemspace);
uintptr_t* load_memspace(struct MemSpace* pmemspace, char* img_start);
struct ThreadStackPointer load_user_stack(struct MemSpace* pmemspace, char** argv);
