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
 * @file:    buddy.c
 * @brief:   the buddy management of system memory
 * @version: 3.0
 * @author:  AIIT XUOS Lab
 * @date:    2023/8/28
 *
 */
/*************************************************
File name: buddy.c
Description: the buddy management of system memory
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

#include "buddy.h"
#include "kalloc.h"
#include "log.h"
#include "pagetable.h"

static void _buddy_split_page(struct KPage* page, uintptr_t low_order, uintptr_t high_order, struct KFreeList* list)
{
    uintptr_t size = (1 << high_order);
    while (high_order > low_order) {
        list--;
        size >>= 1;
        doubleListAddOnHead(&page[size].node, &list->list_head);
        list->n_free_pages++;
        high_order--;
    }
}

__attribute__((always_inline)) static void inline _buddy_set_pages_order(struct KPage* page, int order)
{
    int i;
    struct KPage* p = NULL;
    page->order = order;
    for (i = 1; i < FREE_LIST_INDEX(order); i++) {
        p = page + i;
        p->page_node = page;
    }
}

__attribute__((always_inline)) static void inline _buddy_page_to_vaddr(struct KBuddy* pbuddy, struct KPage* page, char** vaddr)
{
    uintptr_t offset = page - pbuddy->pages;
    *vaddr = (char*)(pbuddy->mem_start + (offset << LEVEL4_PTE_SHIFT));
    return;
}

__attribute__((always_inline)) static void inline _buddy_vaddr_to_page(struct KBuddy* pbuddy, struct KPage** page, char* vaddr)
{
    uintptr_t offset = (uintptr_t)vaddr - pbuddy->mem_start;
    *page = (struct KPage*)(pbuddy->pages + (offset >> LEVEL4_PTE_SHIFT));
    return;
}

// find free page and split it to small page if need
static struct KPage* KBuddyPagesAlloc(struct KBuddy* pbuddy, int nPages)
{
    struct KPage* page = NULL;
    struct KFreeList* list = NULL;
    int i = 0, order = 0;

    // find order
#if defined(__GNUC__)
    // see: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    order = nPages ? (sizeof(int) * 8 - __builtin_clz(nPages) - 1) + !!(__builtin_popcount(nPages) != 1) : 0;
#else
    for (order = 0; (FREE_LIST_INDEX(order)) < nPages; order++)
        ;
#endif

    // find the free page list
    for (i = order; i < MAX_BUDDY_ORDER; i++) {
        list = pbuddy->free_list + i;
        if (IS_DOUBLE_LIST_EMPTY(&list->list_head)) {
            continue;
        }

        // find the page
        page = CONTAINER_OF(list->list_head.next, struct KPage, node);
        doubleListDel(&page->node);
        list->n_free_pages--;

        // split the page to some small pages
        _buddy_split_page(page, order, i, list);

        // set the pages' order
        _buddy_set_pages_order(page, order);

        return page;
    }

    // there is no enough free page to satisfy the nPages
    return NULL;
}

// free continuous pages from page pointer
static void KBuddyPagesFree(struct KBuddy* pbuddy, struct KPage* page)
{
    struct KPage* buddy = NULL;
    uintptr_t order = (page->order >= MAX_BUDDY_ORDER) ? 0 : page->order;
    uintptr_t page_idx = page - pbuddy->pages;

    for (; order < MAX_BUDDY_ORDER - 1; order++) {
        // find and delete buddy to combine
        uintptr_t buddy_idx = BUDDY_PAGE_INDEX(page_idx, order);
        if (buddy_idx > pbuddy->n_pages - 1) {
            break;
        }
        buddy = page + (buddy_idx - page_idx);
        if (!IS_BUDDY_PAGE(buddy, order)) {
            break;
        }
        // remove buddy
        doubleListDel(&buddy->node);
        pbuddy->free_list[order].n_free_pages--;
        buddy->order = MAX_BUDDY_ORDER;
        // update page and page_idx after combined
        uintptr_t new_buddy_idx = COMBINED_PAGE_INDEX(page_idx, order);
        page = page + (new_buddy_idx - page_idx);
        page_idx = new_buddy_idx;
    }
    page->order = order;
    doubleListAddOnHead(&page->node, &pbuddy->free_list[order].list_head);
    pbuddy->free_list[order].n_free_pages++;

    return;
}

bool KBuddyInit(struct KBuddy* pbuddy, uintptr_t mem_start, uintptr_t mem_end)
{
    if (pbuddy->pages == NULL) {
        if ((pbuddy->pages = (struct KPage*)kalloc(((mem_end - mem_start) >> LEVEL4_PTE_SHIFT) * sizeof(struct KPage))) == NULL) {
            ERROR("Not space to init a buddy object.\n");
            return false;
        }
    }

    uintptr_t i = 0;
    struct KPage* page = NULL;
    struct KFreeList* free_list = NULL;

    // init global kernel Buddy system
    pbuddy->mem_start = mem_start;
    pbuddy->mem_end = mem_end;

    // k_page is used to manage free pages
    pbuddy->mem_start = ALIGNUP(pbuddy->mem_start, 4 * PAGE_SIZE);

    // total number of free pages
    pbuddy->n_pages = (pbuddy->mem_end - (uintptr_t)pbuddy->mem_start) >> LEVEL4_PTE_SHIFT;

    memset(pbuddy->pages, 0, pbuddy->n_pages * sizeof(struct KPage));
    // memset(pbuddy->pages, 0, pbuddy->n_pages);

    // init each free page list from 2^0 to 2^8
    for (; i < MAX_BUDDY_ORDER; i++) {
        free_list = pbuddy->free_list + i;
        doubleListNodeInit(&free_list->list_head);
        free_list->n_free_pages = 0;
    }

    // init and free each page
    for (i = 0; i < pbuddy->n_pages; i++) {
        page = pbuddy->pages + i;
        page->order = MAX_BUDDY_ORDER;
    }
    for (i = 0; i < pbuddy->n_pages; i++) {
        page = pbuddy->pages + i;
        doubleListNodeInit(&page->node);
        KBuddyPagesFree(pbuddy, page);
    }

    return true;
}

void KBuddySysInit(struct KBuddy* pbuddy, uintptr_t mem_start, uintptr_t mem_end)
{
#define MAX_NR_PAGES MAX_NR_FREE_PAGES
    static struct KPage kern_free_pages[MAX_NR_PAGES];
    pbuddy->pages = kern_free_pages;
    KBuddyInit(pbuddy, mem_start, mem_end);
}

char* KBuddyAlloc(struct KBuddy* pbuddy, uintptr_t size)
{
    int nPages = CALCULATE_NPAGES(size);
    struct KPage* page = KBuddyPagesAlloc(pbuddy, nPages);
    if (page == NULL)
        return NULL;

    char* v_addr = NULL;
    _buddy_page_to_vaddr(pbuddy, page, &v_addr);

    return v_addr;
}

bool KBuddyFree(struct KBuddy* pbuddy, char* vaddr)
{
    struct KPage* page = NULL;

    if ((uintptr_t)vaddr % (PAGE_SIZE)) {
        ERROR("kbuddyfree - unaligned: %x\n", vaddr);
        return false;
    }
    if ((uintptr_t)vaddr < pbuddy->mem_start) {
        ERROR("kbuddyfree - under buddy free page address: %x\n", vaddr);
        return false;
    }
    if ((uintptr_t)vaddr >= pbuddy->mem_end) {
        ERROR("kbuddyfree - over buddy free page address: %x\n", vaddr);
        return false;
    }

    _buddy_vaddr_to_page(pbuddy, &page, vaddr);
    KBuddyPagesFree(pbuddy, page);

    return true;
}

void KBuddyDestory(struct KBuddy* pbuddy)
{
    if (pbuddy->pages) {
        kfree((void*)pbuddy->pages);
    }
}

void KFreePagesInfo(struct KBuddy* pbuddy)
{
    DEBUG("Buddy structure:");
    for (int j = 0; j < MAX_BUDDY_ORDER; j++) {
        DEBUG_PRINTF(" %d ", pbuddy->free_list[j].n_free_pages);
    }
    DEBUG_PRINTF("\n");
}