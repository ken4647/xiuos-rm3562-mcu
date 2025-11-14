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
* @file:    cache_common_ope.h
* @brief:   the general management of cache
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2023/11/06
*
*/

/*************************************************
File name: cache_common_ope.h
Description: the general management of cache
Others: 
History: 
1. Date: 2023-11-06
Author: AIIT XUOS Lab
Modification: 
1、define xiuos cache interfaces
*************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>

#include "actracer.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

struct ICacheDone {
    void (*enable)(void);
    void (*disable)(void);
    void (*invalidate)(uintptr_t start, uintptr_t end);
    void (*invalidateall)(void);
};

struct DCacheDone {
    void (*enable)(void);
    void (*disable)(void);
    void (*clean)(uintptr_t start, uintptr_t end);
    void (*flush)(uintptr_t start, uintptr_t end);
    void (*invalidate)(uintptr_t start, uintptr_t end);
    void (*cleanall)(void);
    void (*flushall)(void);
    void (*invalidateall)(void);
};

struct ICacheDone* hardkernel_icache_init(struct TraceTag* hardkernel_tag);
struct DCacheDone* hardkernel_dcache_init(struct TraceTag* hardkernel_tag);

void test_cache(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLY__ */


