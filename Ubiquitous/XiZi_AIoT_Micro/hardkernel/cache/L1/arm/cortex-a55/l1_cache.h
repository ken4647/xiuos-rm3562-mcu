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
 * @version: 1.0
 * @author:  AIIT XUOS Lab
 * @date:    2024/4/23
 *
 */

/*************************************************
File name: l1_cache.h
Description: the general management of L1 cache
Others:
History:
Author: AIIT XUOS Lab
Modification:
1、define the l1 cache operations
*************************************************/
#include "core.h"
#include <stdint.h>

/*
 * L1 Cache Operation:
 *
 * IVAC       -Invalidate by Virtual Address, to Point of Coherency       AArch32Equivalent :DCIMVAC
 *
 * ISW        -Invalidate by Set/Way                                                                     AArch32Equivalent :DCISW
 *
 *CVAC     -Clean by Virtual Address to Point of Coherency                  AArch32Equivalent :DCCMVAC
 *
 *CSW       -Clean by Set/Way                                                                              AArch32Equivalent :DCCSW
 *
 *CVAU      -Clean by Virtual Address to Point of Unification                AArch32Equivalent :DCCMVAU
 *
 *CIVAC      -Clean and invalidate data cache line by VA to PoC.         AArch32Equivalent :DCCIMVAC
 *
 *ISW         -Clean and invalidate data cache line by Set/Way.             AArch32Equivalent :DCCISW
 */

#define SCTLR_EL1_ICACHE_ENABLE (1 << 12) //!< Instruction cache enable
#define SCTLR_EL1_DCACHE_ENABLE (1 << 2) //!< Data cache enable

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