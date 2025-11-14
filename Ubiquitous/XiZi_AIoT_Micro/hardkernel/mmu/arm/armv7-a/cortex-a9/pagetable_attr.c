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
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: pagetable_attr.c
Description: mmu entry attributes
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "mmu.h"
#include "mmu_common.h"

void GetUsrPteAttr(uintptr_t* attr)
{
    static char init = 0;
    static PageTblEntry usr_pte_attr;
    if (init == 0) {
        init = 1;

        usr_pte_attr.entry = 0;
        usr_pte_attr.desc_type = PAGE_4K;
        usr_pte_attr.B = 1;
        usr_pte_attr.C = 1;
        usr_pte_attr.S = 1;
        usr_pte_attr.AP1_0 = AccessPermission_KernelUser;
    }
    *attr = usr_pte_attr.entry;
}

void GetUsrDevPteAttr(uintptr_t* attr)
{
    static char init = 0;
    static PageTblEntry usr_pte_attr;
    if (init == 0) {
        init = 1;

        usr_pte_attr.entry = 0;
        usr_pte_attr.desc_type = PAGE_4K;
        usr_pte_attr.TEX = 2;
        // usr_pte_attr.B = 1;
        usr_pte_attr.S = 1;
        usr_pte_attr.AP1_0 = AccessPermission_KernelUser;
    }
    *attr = usr_pte_attr.entry;
}

void GetDevPteAttr(uintptr_t* attr)
{
    static char init = 0;
    static PageTblEntry dev_pte_attr;
    if (init == 0) {
        init = 1;

        dev_pte_attr.entry = 0;
        dev_pte_attr.desc_type = PAGE_4K;
        dev_pte_attr.AP1_0 = AccessPermission_KernelOnly;
    }
    *attr = dev_pte_attr.entry;
}

void GetKernPteAttr(uintptr_t* attr)
{
    static char init = 0;
    static PageTblEntry kern_pte_attr;
    if (init == 0) {
        init = 1;

        kern_pte_attr.entry = 0;
        kern_pte_attr.desc_type = PAGE_4K;
        kern_pte_attr.B = 1;
        kern_pte_attr.C = 1;
        kern_pte_attr.S = 1;
        kern_pte_attr.TEX = 1;
        kern_pte_attr.AP1_0 = AccessPermission_KernelOnly;
    }
    *attr = kern_pte_attr.entry;
}

void GetPdeAttr(uintptr_t* attr)
{
    static char init = 0;
    static PageDirEntry pde_attr;
    if (init == 0) {
        init = 1;

        pde_attr.entry = 0;
        pde_attr.desc_type = PAGE_DIR_COARSE;
    }
    *attr = pde_attr.entry;
}