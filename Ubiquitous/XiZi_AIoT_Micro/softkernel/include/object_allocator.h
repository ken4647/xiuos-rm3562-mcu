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
 * @file object_allocator.h
 * @brief slab algorithm declaration
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: object_allocator.h
Description: slab algorithm declaration
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "actracer_tag.h"
#include <stddef.h>
#include <stdint.h>

struct slab_state {
    TraceTag owner_tag;
    struct slab_state *prev, *next;
    uint64_t bitmap;
    uintptr_t refcount;
    uint8_t data[] __attribute__((aligned(sizeof(void*))));
};

struct slab_allocator {
    size_t element_size;
    size_t nr_elements;
    size_t slabsize;
    uint64_t bitmap_empty;
    struct slab_state *partial, *empty, *full;
    char* name;
};

void slab_init(struct slab_allocator*, size_t, char* name);
void slab_destroy(const struct slab_allocator*);

void* slab_alloc(struct slab_allocator*);
void slab_free(struct slab_allocator*, const void*);
