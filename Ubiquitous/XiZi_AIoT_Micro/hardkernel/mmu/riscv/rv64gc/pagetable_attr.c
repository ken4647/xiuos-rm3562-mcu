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

#include "asm/pgtable-bits.h"

#define _PAGE_KERNEL		(_PAGE_READ \
				| _PAGE_WRITE \
				| _PAGE_PRESENT \
				| _PAGE_ACCESSED \
				| _PAGE_DIRTY \
				| _PAGE_GLOBAL)


void GetUsrPteAttr(uintptr_t* attr)
{
    *attr = _PAGE_KERNEL | _PAGE_USER | _PAGE_EXEC;
}

void GetUsrDevPteAttr(uintptr_t* attr)
{
    *attr = _PAGE_KERNEL | _PAGE_USER;
}

void GetDevPteAttr(uintptr_t* attr)
{
    *attr = _PAGE_KERNEL;
}

void GetKernPteAttr(uintptr_t* attr)
{
    *attr = _PAGE_KERNEL | _PAGE_EXEC;
}

void GetPdeAttr(uintptr_t* attr)
{
    *attr = _PAGE_PRESENT;
}