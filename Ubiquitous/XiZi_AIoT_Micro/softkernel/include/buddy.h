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
 * @file buddy.h
 * @brief buddy algorithm header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: buddy.h
Description: buddy algorithm header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "list.h"
#include "memlayout.h"

#include <stdbool.h>
#include <stdint.h>

#define MAX_BUDDY_ORDER (18)

#define FREE_LIST_INDEX(order) \
    (1 << order)

#define IS_BUDDY_PAGE(buddy, order) \
    (buddy->order == order && buddy->node.next != &buddy->node)

#define BUDDY_PAGE_INDEX(page_idx, order) \
    (page_idx ^ (FREE_LIST_INDEX(order)))

#define COMBINED_PAGE_INDEX(page_idx, order) \
    (page_idx & ~(FREE_LIST_INDEX(order)))

#define CALCULATE_NPAGES(size) \
    (ALIGNUP(size, PAGE_SIZE) >> LEVEL4_PTE_SHIFT)

struct KPage {
    struct double_list_node node;
    union {
        uintptr_t order;
        struct KPage* page_node;
    };
    uintptr_t mapped_addr;
};

struct KFreeList {
    uintptr_t n_free_pages;
    struct double_list_node list_head;
};

struct KBuddy {
    uintptr_t n_pages;
    uintptr_t use_lock;
    struct KFreeList free_list[MAX_BUDDY_ORDER];
    struct KPage* first_page;
    uintptr_t mem_start;
    uintptr_t mem_end;
    struct KPage* pages;
};

/*********************************************
 * Buddy system public functions
 *********************************************/
/*
 * Buddy system init function
 * @param mem_start  free memory region start
 * @param mem_end  free memory region end
 * @return void
 */
bool KBuddyInit(struct KBuddy* pbuddy, uintptr_t mem_start, uintptr_t mem_end);
void KBuddySysInit(struct KBuddy* pbuddy, uintptr_t mem_start, uintptr_t mem_end);

/*
 * Continuous pages alloc by size
 * @param  size（uintptr_t） size of need alloc
 * @return NULL or v_addr (char*) return NULL if alloc failed, or return virtual page's addr
 */
char* KBuddyAlloc(struct KBuddy* pbuddy, uintptr_t size);

/*
 * Continuous pages free from vaddr
 * @param vaddr(char*) virtual addr
 * @param isFreeSuccess(bool) return false if free failed, or return true
 */
bool KBuddyFree(struct KBuddy* pbuddy, char* vaddr);

void KBuddyDestory(struct KBuddy* pbuddy);

/*
 * Print current free pages for debug.
 */
void KFreePagesInfo(struct KBuddy* pbuddy);
