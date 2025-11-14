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
 * @file memspace.c
 * @brief memspace loader
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: memspace.c
Description: memspace loader
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "assert.h"
#include "bitmap64.h"
#include "execelf.h"
#include "kalloc.h"
#include "memspace.h"
#include "task.h"

#define MAX_SUPPORT_PARAMS 32

struct MemSpace* alloc_memspace(char* name)
{
    struct MemSpace* pmemspace = slab_alloc(&xizi_task_manager.memspace_allocator);
    if (pmemspace == NULL) {
        ERROR("Alloc memspace for thread failed.\n");
        return NULL;
    }

    bitmap64_init(&pmemspace->thread_stack_idx_bitmap);
    doubleListNodeInit(&pmemspace->thread_list_guard);
    pmemspace->massive_ipc_allocator = NULL;
    pmemspace->heap_base = 0;
    pmemspace->mem_size = 0;
    pmemspace->pgdir.pd_addr = 0;
    pmemspace->thread_to_notify = NULL;
    if (!CreateResourceTag(&pmemspace->tag, &xizi_task_manager.tag, name, TRACER_OWNER, (void*)pmemspace)) {
        DEBUG("Register MemSpace %s failed\n", name);
        slab_free(&xizi_task_manager.memspace_allocator, (void*)pmemspace);
        return NULL;
    }
    assert(pmemspace->tag.meta != NULL);

    if (!CreateResourceTag(&pmemspace->kernspace_mem_usage.tag, &pmemspace->tag, "MemUsage", TRACER_SYSOBJECT, (void*)&pmemspace->kernspace_mem_usage) || //
        !CreateResourceTag(&pmemspace->userspace_mem_usage.tag, &pmemspace->tag, "UserMemUsage", TRACER_SYSOBJECT, (void*)&pmemspace->userspace_mem_usage) || //
        !CreateResourceTag(&pmemspace->customized_mapping_mem_map.tag, &pmemspace->tag, "CustomizaedMemMapping", TRACER_SYSOBJECT, (void*)&pmemspace->customized_mapping_mem_map)) {
        DEBUG("Register MemUsage %s failed\n", name);
        slab_free(&xizi_task_manager.memspace_allocator, (void*)pmemspace);
        DeleteResource(&pmemspace->tag, &xizi_task_manager.tag);
        return NULL;
    }

    rbtree_init(&pmemspace->kernspace_mem_usage.mem_block_map);
    rbtree_init(&pmemspace->userspace_mem_usage.mem_block_map);
    rbtree_init(&pmemspace->customized_mapping_mem_map.mem_block_map);
    return pmemspace;
}

void free_memspace(struct MemSpace* pmemspace)
{
    assert(pmemspace != NULL);
    assert(IS_DOUBLE_LIST_EMPTY(&pmemspace->thread_list_guard));

    /* free page table and all its allocated memories */
    if (pmemspace->pgdir.pd_addr != NULL) {
        xizi_pager.free_user_pgdir(&pmemspace->pgdir);
    }

    // delete space
    RbtNode* rbt_node = pmemspace->kernspace_mem_usage.mem_block_map.root;
    while (rbt_node != NULL) {
        assert((uintptr_t)V2P(rbt_node->key) >= PHY_MEM_BASE && (uintptr_t)V2P(rbt_node->key) < PHY_MEM_STOP);
        kfree_by_ownership(pmemspace->kernspace_mem_usage.tag, (void*)rbt_node->key);
        rbt_node = pmemspace->kernspace_mem_usage.mem_block_map.root;
    }

    rbt_node = pmemspace->userspace_mem_usage.mem_block_map.root;
    while (rbt_node != NULL) {
        assert((uintptr_t)rbt_node->key >= PHY_MEM_BASE && (uintptr_t)rbt_node->key < PHY_MEM_STOP);
        raw_free_by_ownership(pmemspace->userspace_mem_usage.tag, (void*)rbt_node->key);
        rbt_node = pmemspace->userspace_mem_usage.mem_block_map.root;
    }

    /* free ipc virt address allocator */
    if (pmemspace->massive_ipc_allocator != NULL) {
        KBuddyDestory(pmemspace->massive_ipc_allocator);
        slab_free(&xizi_task_manager.task_buddy_allocator, (void*)pmemspace->massive_ipc_allocator);
    }

    DeleteResource(&pmemspace->tag, &xizi_task_manager.tag);
    slab_free(&xizi_task_manager.memspace_allocator, (void*)pmemspace);
}

/// @return return the entry of program
uintptr_t* load_memspace(struct MemSpace* pmemspace, char* img_start)
{
    if (pmemspace == NULL) {
        ERROR("Loading an empty memspace.\n");
        return NULL;
    }

    if (img_start == NULL) {
        ERROR("Empty elf file.\n");
        return NULL;
    }

    /* 1. load elf header */
    struct elfhdr elf;
    memcpy((void*)&elf, img_start, sizeof(elf));
    if (elf.magic != ELF_MAGIC) {
        ERROR("Not an elf file.\n");
        return NULL;
    }

    /* allocate a pgdir */
    /* only supports first inited memspace */
    assert(pmemspace->pgdir.pd_addr == NULL);
    if (UNLIKELY(!xizi_pager.new_pgdir(&pmemspace->pgdir))) {
        ERROR("Create new pgdir failed.\n");
        goto error_exec;
    }
    /* copy kernel pagetable so that interrupt and syscall wont corrupt */
    memcpy(pmemspace->pgdir.pd_addr, kern_pgdir.pd_addr, TOPLEVLE_PAGEDIR_SIZE);
#ifdef __riscv
    xizi_pager.new_pgdir(&pmemspace->pgdir_riscv);
    memcpy(pmemspace->pgdir_riscv.pd_addr, kern_pgdir.pd_addr, TOPLEVLE_PAGEDIR_SIZE);
#endif

    // read elf file by (header, section)
    uintptr_t load_size = 0;
    struct proghdr ph;
    for (int sec_idx = 0, off = elf.phoff; sec_idx < elf.phnum; sec_idx++, off += sizeof(ph)) {
        // load proghdr
        memcpy((char*)&ph, img_start + off, sizeof(ph));

        if (ph.type != ELF_PROG_LOAD)
            continue;
        if (ph.memsz < ph.filesz) {
            ERROR("elf header mem size less than file size\n");
            goto error_exec;
        }

        // read section
        // 1. alloc space
        if ((load_size = xizi_pager.resize_user_pgdir(pmemspace, load_size, ph.vaddr + ph.memsz))
            != ph.vaddr + ph.memsz) {
            ERROR("Add uspace size failed.\n");
            goto error_exec;
        }
        // 2. copy inode to space
        if (ph.vaddr % PAGE_SIZE != 0) {
            LOG("Unsupported elf file, try use flag -N to compile.\n");
        }

        for (int addr_offset = 0; addr_offset < ph.filesz; addr_offset += PAGE_SIZE) {
            uintptr_t page_paddr = xizi_pager.address_translate(&pmemspace->pgdir, ph.vaddr + addr_offset);
            if (page_paddr == 0) {
                ERROR("copy elf file to unmapped addr: %x(pgdir: %x)\n", ph.vaddr + addr_offset, pmemspace->pgdir.pd_addr);
                goto error_exec;
            }
            uintptr_t read_size = (ph.filesz - addr_offset < PAGE_SIZE ? ph.filesz - addr_offset : PAGE_SIZE);
            memcpy(P2V(page_paddr), img_start + (ph.off + addr_offset), read_size);
        }
    }

    /// elf file content now in memory
    // memspace will use this page dir
    pmemspace->heap_base = ALIGNUP(load_size, PAGE_SIZE);
    pmemspace->mem_size = pmemspace->heap_base;

    return (uintptr_t*)elf.entry;

error_exec:
    if (pmemspace->pgdir.pd_addr != NULL) {
        xizi_pager.free_user_pgdir(&pmemspace->pgdir);
        pmemspace->pgdir.pd_addr = NULL;
    }
    ERROR("Error loading memspace.\n");
    return NULL;
}

static void handle_error_stack_loading(struct MemSpace* pmemspace, int stack_idx, uintptr_t* stack_bottom, bool is_mapped_successful)
{
    if (stack_idx != -1) {
        bitmap64_free(&pmemspace->thread_stack_idx_bitmap, stack_idx);
    }

    if (stack_bottom != NULL) {
        kfree((char*)stack_bottom);
    }

    if (is_mapped_successful) {
        xizi_pager.unmap_pages(pmemspace->pgdir.pd_addr, USER_MEM_TOP - ((stack_idx + 1) * USER_STACK_SIZE), USER_STACK_SIZE);
    }
}

/// @return return thread's user stack index
struct ThreadStackPointer load_user_stack(struct MemSpace* pmemspace, char** argv)
{
    /* usages of load_user_stack() must be correct */
    assert(pmemspace != NULL);
    assert(pmemspace->pgdir.pd_addr != NULL);
    assert(argv != NULL);

    struct ThreadStackPointer loaded_sp = {
        .argc = 0,
        .stack_idx = -1,
        .user_sp = 0,
        .user_stack_vaddr = 0,
    };

    /* alloc a user stack index */
    int stack_idx = bitmap64_alloc(&pmemspace->thread_stack_idx_bitmap);
    if (stack_idx == -1) {
        ERROR("Number of threads created exceeds kernel support.\n");
        handle_error_stack_loading(pmemspace, stack_idx, NULL, false);
        return loaded_sp;
    }

    /* allocate memory space for user stack */
    uintptr_t* stack_bottom = (uintptr_t*)kalloc(USER_STACK_SIZE);
    if (UNLIKELY(stack_bottom == NULL)) {
        ERROR("No memory to alloc user stack.\n");
        handle_error_stack_loading(pmemspace, stack_idx, stack_bottom, false);
        return loaded_sp;
    }

    /* map memory to user stack space in memspace*/
    if (!xizi_pager.map_pages(pmemspace, USER_MEM_TOP - ((stack_idx + 1) * USER_STACK_SIZE), V2P(stack_bottom), USER_STACK_SIZE, false)) {
        /* this could only fail due to inner page directory's allocation failure */
        ERROR("User stack map failed\n");
        handle_error_stack_loading(pmemspace, stack_idx, stack_bottom, false);
        return loaded_sp;
    }

    /* start loading main params into user stack */
    /// @warning supports only main style params
    uintptr_t user_vspace_sp = USER_MEM_TOP - (stack_idx * USER_STACK_SIZE);
    static uintptr_t user_stack_init[MAX_SUPPORT_PARAMS];
    memset(user_stack_init, 0, sizeof(user_stack_init));
    uintptr_t argc = 0;
    uintptr_t copy_len = 0;
    for (argc = 0; argv != NULL && argc < (MAX_SUPPORT_PARAMS - 1) && argv[argc] != NULL; argc++) {
        /// @todo handle with large number of parameters (more than 32)
        // copy param to user stack
        copy_len = strlen(argv[argc]) + 1;
        user_vspace_sp = ALIGNDOWN(user_vspace_sp - copy_len, sizeof(uintptr_t));
        uintptr_t copied_len = xizi_pager.cross_vspace_data_copy(&pmemspace->pgdir, user_vspace_sp, (uintptr_t)argv[argc], copy_len);
        if (UNLIKELY(copied_len != copy_len)) {
            ERROR("Something went wrong when copying params.\n");
            handle_error_stack_loading(pmemspace, stack_idx, stack_bottom, true);
            return loaded_sp;
        }
        user_stack_init[argc] = user_vspace_sp;
    }

    user_stack_init[argc] = 0;
    copy_len = (argc + 1) * sizeof(uintptr_t);
    user_vspace_sp -= copy_len;
    /* this copy has no reason to fail */
    uintptr_t copied_len = xizi_pager.cross_vspace_data_copy(&pmemspace->pgdir, user_vspace_sp, (uintptr_t)user_stack_init, copy_len);
    assert(copied_len == copy_len);

    pmemspace->mem_size += USER_STACK_SIZE;

    loaded_sp.argc = argc;
    loaded_sp.user_sp = user_vspace_sp;
    loaded_sp.user_stack_vaddr = (uintptr_t)stack_bottom;
    loaded_sp.stack_idx = stack_idx;
    return loaded_sp;
}