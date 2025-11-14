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
 * @file:    cache_common_ope.c
 * @brief:   the general management of cache
 * @version: 3.0
 * @author:  AIIT XUOS Lab
 * @date:    2023/11/06
 *
 */

/*************************************************
File name: cache_common_ope.c
Description: the general management of cache
Others:
History:
1. Date: 2023-11-06
Author: AIIT XUOS Lab
Modification:
1、implement xiuos cache operations
*************************************************/
#include "cache_common_ope.h"
#include "l1_cache.h"
// #include "l2_cache.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/
/****************************************************************************
 * Name: invalidate_dcache
 *
 * Description:
 *   Invalidate the data cache within the specified region; we will be
 *   performing a DMA operation in this region and we want to purge old data
 *   in the cache.
 *
 ****************************************************************************/

static inline void invalidate_dcache(uintptr_t start, uintptr_t end)
{
    // InvalidateL1Dcache(start, end);
    // InvalidateL2Cache(start, end);
}

/****************************************************************************
 * Name: invalidate_dcache_all
 *
 * Description:
 *   Invalidate the entire contents of D cache.
 *
 *   NOTE: This function forces L1 and L2 cache operations to be atomic
 *   by disabling interrupts.
 *
 ****************************************************************************/

static inline void invalidate_dcache_all(void)
{
    // InvalidateL1DcacheAll();
    // InvalidateL2CacheAll();
}

/****************************************************************************
 * Name: invalidate_icache
 *
 * Description:
 *   Invalidate the instruction cache within the specified region.
 *
 ****************************************************************************/
static inline void invalidate_icache(uintptr_t start, uintptr_t end)
{
    // InvalidateL1Icache(start, end);
}

/****************************************************************************
 * Name: invalidate_icache_all
 *
 * Description:
 *   Invalidate all instruction caches to PoU, also flushes branch target
 *   cache
 *
 ****************************************************************************/

static inline void invalidate_icache_all(void)
{
    InvalidateL1IcacheAll();
}

/****************************************************************************
 * Name: clean_dcache
 *
 * Description:
 *   Clean the data cache within the specified region by flushing the
 *   contents of the data cache to memory.
 *
 ****************************************************************************/

static inline void clean_dcache(uintptr_t start, uintptr_t end)
{
    // CleanL1Dcache(start, end);
    // CleanL2Cache(start, end);
}

/****************************************************************************
 * Name: clean_dcache_all
 *
 * Description:
 *   Clean the entire data cache within the specified region by flushing the
 *   contents of the data cache to memory.
 *
 ****************************************************************************/

static inline void clean_dcache_all(void)
{
    // CleanL1DcacheAll();
    // CleanL2CacheAll();
}

/****************************************************************************
 * Name: flush_dcache
 *
 * Description:
 *   Flush the data cache within the specified region by cleaning and
 *   invalidating the D cache.
 *
 ****************************************************************************/

static inline void flush_dcache(uintptr_t start, uintptr_t end)
{

    // FlushL1Dcache(start, end);
    // FlushL2Cache(start, end);
}

/****************************************************************************
 * Name: flush_dcache_all
 *
 * Description:
 *   Flush the entire data cache by cleaning and invalidating the D cache.
 *
 ****************************************************************************/

static inline void flush_dcache_all(void)
{
    FlushL1DcacheAll();
    // FlushL2CacheAll();
}

/****************************************************************************
 * Name: enable_icache
 *
 * Description:
 *   Enable the I-Cache
 *
 ****************************************************************************/

static inline void enable_icache(void)
{
    // EnableL1Icache();
}

/****************************************************************************
 * Name: disable_icache
 *
 * Description:
 *   Disable the I-Cache
 *
 ****************************************************************************/

static inline void disable_icache(void)
{
    // DisableL1Icache();
}

/****************************************************************************
 * Name: enable_dcache
 *
 * Description:
 *   Enable the D-Cache
 *
 ****************************************************************************/

static inline void enable_dcache(void)
{
    // EnableL1Dcache();
    // EnableL2Cache();
}

/****************************************************************************
 * Name: disable_dcache
 *
 * Description:
 *   Disable the D-Cache
 *
 ****************************************************************************/

static inline void disable_dcache(void)
{
    // FlushL1DcacheAll();
    // pl310_flush_all();
    // DisableL1Dcache();
    // DisableL2Cache();
}

static struct ICacheDone icache_done = {
    .enable = enable_icache,
    .disable = disable_icache,
    .invalidate = invalidate_icache,
    .invalidateall = invalidate_icache_all,
};

static struct DCacheDone dcache_done = {
    .enable = enable_dcache,
    .disable = disable_dcache,
    .clean = clean_dcache,
    .flush = flush_dcache,
    .invalidate = invalidate_dcache,
    .cleanall = clean_dcache_all,
    .flushall = flush_dcache_all,
    .invalidateall = invalidate_dcache_all,
};

struct ICacheDone* hardkernel_icache_init(struct TraceTag* hardkernel_tag)
{
    /* init icache */
    icache_done.enable();
    return &icache_done;
}

struct DCacheDone* hardkernel_dcache_init(struct TraceTag* hardkernel_tag)
{
    /* init dcache */
    dcache_done.enable();
    return &dcache_done;
}