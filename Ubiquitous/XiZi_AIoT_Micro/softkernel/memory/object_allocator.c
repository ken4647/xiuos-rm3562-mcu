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
 * @file object_allocator.c
 * @brief slab implementation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: object_allocator.c
Description: slab implementation
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stddef.h>

#include "assert.h"
#include "kalloc.h"
#include "object_allocator.h"
#include "pagetable.h"

#define BITMAP_BITS_EMPTY_FULL ((uint64_t)0)
#define BITMAP_FIRST_BIT ((uint64_t)1)

#define BITMAP_FIND_FIRST_BIT(s) ((size_t)__builtin_ctzll(s))
#define BITMAP_FREE_BIT(s) ((size_t)__builtin_popcountll(s))
#define BITMAP_USED_SOLE_BIT(bitmap, bitmap_empty) \
    (((~(bitmap) & (bitmap_empty)) & ((~(bitmap) & (bitmap_empty)) - 1)) == BITMAP_BITS_EMPTY_FULL)

#define LOWLEVEL_ALLOC(size) kalloc(size)
#define LOWLEVEL_FREE(ptr) kfree(ptr)

#define ARENA_SIZE_PER_INCREASE (2 * PAGE_SIZE)
#define MAX_NR_ELEMENT_PER_SLABPAGE 64

void slab_init(struct slab_allocator* const allocator, const size_t element_size, char* name)
{
    if (allocator == NULL) {
        panic("init a NULL slab_allocator\n");
    }
    if (element_size > ARENA_SIZE_PER_INCREASE) {
        panic("Not supported slab element size\n");
    }

    allocator->element_size = element_size;
    allocator->slabsize = ARENA_SIZE_PER_INCREASE;

    allocator->nr_elements = (allocator->slabsize - offsetof(struct slab_state, data)) / element_size;
    allocator->nr_elements = allocator->nr_elements > MAX_NR_ELEMENT_PER_SLABPAGE ? MAX_NR_ELEMENT_PER_SLABPAGE : allocator->nr_elements;

    allocator->bitmap_empty = ~BITMAP_BITS_EMPTY_FULL >> (MAX_NR_ELEMENT_PER_SLABPAGE - allocator->nr_elements);
    allocator->partial = allocator->empty = allocator->full = NULL;

    if (name) {
        allocator->name = name;
    }
}

void* slab_alloc(struct slab_allocator* const allocator)
{
    assert(allocator != NULL);

    if (LIKELY(allocator->partial != NULL)) {
        register const size_t slot = BITMAP_FIND_FIRST_BIT(allocator->partial->bitmap);
        allocator->partial->bitmap ^= BITMAP_FIRST_BIT << slot;

        if (UNLIKELY(allocator->partial->bitmap == BITMAP_BITS_EMPTY_FULL)) {
            struct slab_state* const full_head = allocator->partial;
            if ((allocator->partial = allocator->partial->next) != NULL) {
                allocator->partial->prev = NULL;
            }
            if ((full_head->next = allocator->full) != NULL) {
                allocator->full->prev = full_head;
            }
            allocator->full = full_head;

            void* return_addr = allocator->full->data + slot * allocator->element_size;
            memset(return_addr, 0, allocator->element_size);
            return return_addr;
        } else {
            void* return_addr = allocator->partial->data + slot * allocator->element_size;
            memset(return_addr, 0, allocator->element_size);
            return return_addr;
        }
    }

    /* there is no partial slab */
    if (LIKELY((allocator->partial = allocator->empty) != NULL)) {
        /* achieve (partial) slab from empty */
        if (LIKELY((allocator->empty = allocator->empty->next) != NULL)) {
            allocator->empty->prev = NULL;
        }
        allocator->partial->next = NULL;
        allocator->partial->refcount++;
    } else {
        /* achieve slab from outer arena */
        allocator->partial = (struct slab_state*)LOWLEVEL_ALLOC(allocator->slabsize);
        if (UNLIKELY(allocator->partial == NULL)) {
            ERROR("slab %s: no enough memory\n", allocator->name);
            return allocator->partial = NULL;
        }
        allocator->partial->prev = allocator->partial->next = NULL;
        allocator->partial->refcount = 1;
    }
    allocator->partial->bitmap = allocator->bitmap_empty ^ BITMAP_FIRST_BIT;
    assert(allocator->partial->data != NULL);
    memset(allocator->partial->data, 0, allocator->element_size);
    return allocator->partial->data;
}

void slab_free(struct slab_allocator* const allocator, const void* const addr)
{
    assert(allocator != NULL);
    assert(addr != NULL);

    struct slab_state* const slab_to_free = (void*)ALIGNDOWN(addr, allocator->slabsize);
    register const size_t slot = ((char*)addr - (char*)slab_to_free - offsetof(struct slab_state, data)) / allocator->element_size;

    if (UNLIKELY(slab_to_free->bitmap == BITMAP_BITS_EMPTY_FULL)) {
        /* free element from full slab */
        slab_to_free->bitmap = BITMAP_FIRST_BIT << slot;

        if (LIKELY(slab_to_free != allocator->full)) {
            if (LIKELY((slab_to_free->prev->next = slab_to_free->next) != NULL)) {
                slab_to_free->next->prev = slab_to_free->prev;
            }
            slab_to_free->prev = NULL;
        } else if ((allocator->full = allocator->full->next) != NULL) {
            allocator->full->prev = NULL;
        }

        slab_to_free->next = allocator->partial;
        if (LIKELY(allocator->partial != NULL)) {
            allocator->partial->prev = slab_to_free;
        }
        allocator->partial = slab_to_free;

    } else if (UNLIKELY(BITMAP_USED_SOLE_BIT(slab_to_free->bitmap, allocator->bitmap_empty))) {
        /* free element from partial slab that used the  element going to be freed */
        if (LIKELY(slab_to_free->refcount == 1)) {
            if (LIKELY(slab_to_free != allocator->partial)) {
                if ((slab_to_free->prev->next = slab_to_free->next) != NULL) {
                    slab_to_free->next->prev = slab_to_free->prev;
                }
            } else if (LIKELY((allocator->partial = allocator->partial->next) != NULL)) {
                allocator->partial->prev = NULL;
            }

            LOWLEVEL_FREE((void*)slab_to_free);
        } else {
            slab_to_free->bitmap = allocator->bitmap_empty;

            if (LIKELY(slab_to_free != allocator->partial)) {
                if ((slab_to_free->prev->next = slab_to_free->next) != NULL) {
                    slab_to_free->next->prev = slab_to_free->prev;
                }
                slab_to_free->prev = NULL;
            } else if (LIKELY((allocator->partial = allocator->partial->next) != NULL)) {
                allocator->partial->prev = NULL;
            }

            slab_to_free->next = allocator->empty;
            if (UNLIKELY(allocator->empty != NULL))
                allocator->empty->prev = slab_to_free;
            allocator->empty = slab_to_free;
            slab_to_free->refcount--;
        }
    } else {
        slab_to_free->bitmap |= BITMAP_FIRST_BIT << slot;
    }
}

void slab_destroy(const struct slab_allocator* const allocator)
{
    assert(allocator != NULL);

    struct slab_state* const slab_lists[] = { allocator->partial, allocator->empty, allocator->full };
    for (size_t i = 0; i < (sizeof(slab_lists) / sizeof(struct slab_state*)); i++) {
        struct slab_state* slab = slab_lists[i];

        while (slab != NULL) {
            if (slab->refcount != 0) {
                LOWLEVEL_FREE((void*)slab);
            } else {
                slab = slab->next;
            }
        }
    }
}