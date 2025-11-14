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
 * @file kalloc.c
 * @brief kalloc, kfree kind of memory allocation support
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: kalloc.c
Description: kalloc, kfree kind of memory allocation support
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "actracer.h"
#include "rbtree.h"

struct MemUsage {
    TraceTag tag;
    RbtTree mem_block_map;
};

bool module_phymem_init();
char* kalloc(size_t size);
bool kfree(char* vaddr);
bool raw_kfree(char* paddr);

void* kalloc_by_ownership(TraceTag owner, uintptr_t size);
bool kfree_by_ownership(TraceTag owner, void* vaddr);

char* raw_alloc(size_t size);
bool raw_free(char* paddr);
void* raw_alloc_by_ownership(TraceTag owner, uintptr_t size);
bool raw_free_by_ownership(TraceTag owner, void* vaddr);

void show_phymem_info();

extern struct KBuddy user_phy_freemem_buddy;
extern struct KBuddy kern_virtmem_buddy;