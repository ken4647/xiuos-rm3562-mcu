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
 * @file sys_mmap.c
 * @brief map task phy address to virt address
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_mmap.c
Description: map task phy address to virt address
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stdint.h>

#include "assert.h"
#include "kalloc.h"
#include "multicores.h"
#include "share_page.h"
#include "syscall.h"
#include "task.h"

int sys_mmap(uintptr_t* vaddr, uintptr_t* paddr, int len, int is_dev)
{
    struct Thread* cur_task = cur_cpu()->task;
    assert(cur_task != NULL);

    int true_len = ALIGNUP(len, PAGE_SIZE);

    if (*paddr != (uintptr_t)NULL) {
        if (*paddr >= PHY_MEM_BASE && *paddr < PHY_MEM_STOP && cur_task->tid > 1) {
            ERROR("mapping invalid memory: 0x%p\n", *paddr);
            return -1;
        }

        if (xizi_share_page_manager.task_map_pages(cur_task, *vaddr, *paddr, true_len / PAGE_SIZE, is_dev) == (uintptr_t)NULL) {
            return -1;
        }
    } else {
        uintptr_t load_vaddr = *vaddr;
        char* new_paddr = raw_alloc_by_ownership(cur_task->memspace->userspace_mem_usage.tag, true_len);
        if (new_paddr == NULL) {
            return -1;
        }
        if (xizi_share_page_manager.task_map_pages(cur_task, load_vaddr, (uintptr_t)new_paddr, true_len / PAGE_SIZE, false) == (uintptr_t)NULL) {
            raw_free_by_ownership(cur_task->memspace->userspace_mem_usage.tag, new_paddr);
            return -1;
        }
        CreateResourceTag(NULL, &cur_task->memspace->tag, "USER_MEMORY", TRACER_MEM_SIGNATURE, new_paddr);
        *paddr = (uintptr_t)new_paddr;
    }

    cur_task->memspace->mem_size += true_len;
    *vaddr = *vaddr + true_len;
    return 0;
}

extern bool _map_customizable_page(struct MemSpace* pmemspace, uintptr_t vaddr, uintptr_t paddr, int len, uintptr_t attr);
int sys_mmap_v2(uintptr_t* vaddr, uintptr_t* paddr, int len, sys_mmap_info* info)
{
    struct Thread* cur_task = cur_cpu()->task;
    assert(cur_task != NULL);

    if (vaddr == NULL) {
        ERROR("Invalid vaddr from %s\n", cur_task->name);
    }

    int true_len = ALIGNUP(len, PAGE_SIZE);

    sys_mmap_type type = info->type;
    uintptr_t vaddr_to_map = *vaddr;
    if (type == SYS_MMAP_CUSTOMIZE) {
        if (paddr == NULL || *paddr == (uintptr_t)NULL || vaddr_to_map == (uintptr_t)NULL) {
            ERROR("Customized mapping from %s must have vaddr(%p) and paddr(%p)\n", cur_task->name, vaddr, paddr);
            return -1;
        }
        uintptr_t paddr_to_map = *paddr;

        TraceTag mem_signature_tag;
        if (!CreateResourceTag(&mem_signature_tag, &cur_task->memspace->tag, "CUSTOMIZED_MEMORY", TRACER_MEM_SIGNATURE, (void*)vaddr)) {
            ERROR("Sign memory signature failed from %s\n", cur_task->name);
            return -1;
        }

        if (!_map_customizable_page(cur_task->memspace, vaddr_to_map, paddr_to_map, len, info->attr)) {
            ERROR("%s mapping page failed(Short of memory)\n", cur_task->name);
            DeleteResource(&mem_signature_tag, &cur_task->memspace->tag);
            return -1;
        }

        return 0;
    }

    if (type == SYS_MMAP_NORMAL) {
        bool is_dev = info->is_dev;

        if (*paddr != (uintptr_t)NULL) {

            if (paddr == NULL || *paddr == (uintptr_t)NULL || vaddr_to_map == (uintptr_t)NULL) {
                ERROR("Invalid mapping from %s\n", cur_task->name);
                return -1;
            }

            uintptr_t paddr_to_map = *paddr;
            if (paddr_to_map >= PHY_MEM_BASE && paddr_to_map < PHY_MEM_STOP && cur_task->tid > 2) {
                ERROR("mapping invalid memory: 0x%p by %d\n", paddr_to_map, cur_task->tid);
                return -1;
            }

            if (xizi_share_page_manager.task_map_pages(cur_task, vaddr_to_map, paddr_to_map, true_len / PAGE_SIZE, is_dev) == (uintptr_t)NULL) {
                ERROR("%s mapping page failed(Short of memory)\n", cur_task->name);
                return -1;
            }

        } else {
            char* new_paddr = raw_alloc_by_ownership(cur_task->memspace->userspace_mem_usage.tag, true_len);
            if (new_paddr == NULL) {
                ERROR("Alloc dynamic memory failed\n");
                return -1;
            }

            TraceTag mem_signature_tag;
            if (!CreateResourceTag(&mem_signature_tag, &cur_task->memspace->tag, "USER_MEMORY", TRACER_MEM_SIGNATURE, new_paddr)) {
                raw_free_by_ownership(cur_task->memspace->userspace_mem_usage.tag, new_paddr);
                ERROR("Sign memory signature failed from %s\n", cur_task->name);
                return -1;
            }

            if (xizi_share_page_manager.task_map_pages(cur_task, vaddr_to_map, (uintptr_t)new_paddr, true_len / PAGE_SIZE, false) == (uintptr_t)NULL) {
                raw_free_by_ownership(cur_task->memspace->userspace_mem_usage.tag, new_paddr);
                DeleteResource(&mem_signature_tag, &cur_task->memspace->tag);
                return -1;
            }

            // assign new_paddr back to user
            *paddr = (uintptr_t)new_paddr;
        }

        cur_task->memspace->mem_size += true_len;
        *vaddr = *vaddr + true_len;
        return 0;
    }

    return -1;
}