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
* @file:    l1_cache.h
* @brief:   the general management of L1 cache
* @version: 3.0
* @author:  AIIT XUOS Lab
* @date:    2023/4/27
*
*/

/*************************************************
File name: l1_cache.h
Description: the general management of L1 cache
Others: 
History: 
1. Date: 2023-04-27
Author: AIIT XUOS Lab
Modification: 
1、define the l1 cache operations
*************************************************/

#include "core.h"
#include <stdint.h>

/*
 * L1 Cache Registers:
 * 
 * Register:     DCIMVAC  - Invalidate data cache line by VA to PoC.
 * 
 * Register:     DCISW    - Invalidate data cache line by Set/Way.
 * 
 * Register:     DCCMVAC  - Clean data cache line to PoC by VA.
 * 
 * Register:     DCCSW    - Clean data cache line by Set/Way.
 * 
 * Register:     DCCMVAU  - Clean data or unified cache line by VA to PoU.
 * 
 * Register:     DCCIMVAC - Clean and invalidate data cache line by VA to PoC.
 * 
 * Register:     DCCISW   - Clean and invalidate data cache line by Set/Way.
 */

#define BM_SCTLR_I (1 << 12)   //!< Instruction cache enable
#define BM_SCTLR_C (1 << 2)    //!< Data cache enable

void InvalidateL1Dcache(uintptr_t start, uintptr_t end);

void InvalidateL1DcacheAll(void);


void CleanL1Dcache(uintptr_t start, uintptr_t end);

void CleanL1DcacheAll(void);

void FlushL1Dcache(uintptr_t start, uintptr_t end);

void FlushL1DcacheAll(void);


void InvalidateL1IcacheAll(void);

void InvalidateL1Icache(uintptr_t start, uintptr_t end);

void EnableL1Icache(void);
void DisableL1Icache();

void EnableL1Dcache();

void DisableL1Dcache();