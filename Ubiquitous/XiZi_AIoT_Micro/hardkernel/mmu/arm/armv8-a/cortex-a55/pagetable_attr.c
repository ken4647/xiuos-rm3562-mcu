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
 * @file pagetable_attr.c
 * @brief mmu entry attributes
 * @version 1.
 * @author AIIT XUOS Lab
 * @date 2023.04.26
 */

/*************************************************
File name: pagetable_attr.c
Description: mmu entry attributes
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "mmu.h"
#include "mmu_common.h"

// clang-format off
#define ARMV8_PTE_ATTR_MASK(attr)   (((attr) & 0b111) << 2)
#define ARMV8_PTE_DEVICE            ARMV8_PTE_ATTR_MASK(0x0)
#define ARMV8_PTE_NORMAL            ARMV8_PTE_ATTR_MASK(0x1)

#define ARMV8_PTE_AP(ap)            (((ap) & 0b11) << 6)
#define ARMV8_PTE_AP_U              ARMV8_PTE_AP(0x01)
#define ARMV8_PTE_AP_K              ARMV8_PTE_AP(0x00)
#define ARMV8_PTE_AP_RO             ARMV8_PTE_AP(0b10)
#define ARMV8_PTE_AP_RW             ARMV8_PTE_AP(0b00)

#define ARMV8_PTE_AF                (0x1 << 10)
#define ARMV8_PTE_PXN               (1ULL << 53) // Privileged eXecute Never
#define ARMV8_PTE_UXN               (1ULL << 54) // Unprivileged(user) eXecute Never
#define ARMV8_PTE_XN                (ARMV8_PTE_PXN | ARMV8_PTE_UXN)

#define ARMV8_PTE_VALID             (0b11 << 0)
#define ARMV8_PDE_VALID             (0b11 << 0)

// clang-format on

void GetUsrPteAttr(uintptr_t* attr)
{
    // *attr = ARMV8_PTE_AP_U | ARMV8_PTE_AP_RW | ARMV8_PTE_AF | ARMV8_PTE_NORMAL | ARMV8_PTE_VALID;
    *attr = 0x713 | ARMV8_PTE_AP_U;
}

void GetUsrDevPteAttr(uintptr_t* attr)
{
    // *attr = ARMV8_PTE_AP_U | ARMV8_PTE_AP_RW | ARMV8_PTE_AF | ARMV8_PTE_DEVICE | ARMV8_PTE_XN | ARMV8_PTE_VALID;
    *attr = 0x403 | ARMV8_PTE_AP_U;
}

void GetDevPteAttr(uintptr_t* attr)
{
    // *attr = ARMV8_PTE_AP_K | ARMV8_PTE_AP_RW | ARMV8_PTE_AF | ARMV8_PTE_DEVICE | ARMV8_PTE_XN | ARMV8_PTE_VALID;
    *attr = 0x403ULL;
}

void GetKernPteAttr(uintptr_t* attr)
{
    // *attr = ARMV8_PTE_AP_K | ARMV8_PTE_AP_RW | ARMV8_PTE_AF | ARMV8_PTE_NORMAL | ARMV8_PTE_VALID;
    *attr = 0x713ULL;
}

void GetPdeAttr(uintptr_t* attr)
{
    *attr = ARMV8_PDE_VALID;
}