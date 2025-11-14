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
 * @file bitmap64.h
 * @brief 64 bit bitmap support
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: bitmap64.h
Description: 64 bit bitmap support
Others:
History:
1. Date: 2024-05-18
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "assert.h"

struct bitmap64 {
    uint64_t map;
};

static inline void bitmap64_init(struct bitmap64* bitmap)
{
    bitmap->map = 0;
}

static inline int bitmap64_alloc(struct bitmap64* bitmap)
{
    int free_bit = -1;
    // free bit is the first 0 bit, from [0, 63]
    free_bit = __builtin_ffsl(~(uint64_t)(bitmap->map));
    // handle if bitmap is full (no using 64th bit here)
    if (free_bit == 0) {
        return -1;
    }
    free_bit -= 1;
    assert(free_bit < 64 && free_bit >= 0);
    // alloc and return
    bitmap->map |= (1ULL << free_bit);
    return free_bit;
}

static inline void bitmap64_free(struct bitmap64* bitmap, int idx)
{
    // usages of bitmap64 must be correct
    assert((bitmap->map & (1ULL << idx)) != 0);
    // free bit
    bitmap->map &= ~(uint64_t)(1ULL << idx);
}
