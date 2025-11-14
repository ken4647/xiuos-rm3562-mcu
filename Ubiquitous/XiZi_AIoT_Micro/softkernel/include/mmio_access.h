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
 * @file mmio_access.h
 * @brief map phy mmio address t virt mmio address
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: mmio_access.h
Description: map phy mmio address t virt mmio address
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include <stdint.h>

#include "memlayout.h"

#define MMIO_P2V_WO(paddr) ((DEV_VRTMEM_BASE - DEV_PHYMEM_BASE) + (paddr))

#ifndef __ASSEMBLER__
typedef uintptr_t mmio_reg_t;
__attribute__((unused, always_inline)) static inline uintptr_t MMIO_P2V(uintptr_t paddr)
{
    /// @todo use dtb rather than just mapping.
    return (DEV_VRTMEM_BASE - DEV_PHYMEM_BASE) + paddr;
}

__attribute__((unused, always_inline)) static inline void mmio_write(uintptr_t paddr, mmio_reg_t value)
{
    *(volatile mmio_reg_t*)(MMIO_P2V(paddr)) = (value);
}

__attribute__((unused, always_inline)) static inline mmio_reg_t mmio_read(uintptr_t paddr)
{
    return *(volatile mmio_reg_t*)(MMIO_P2V(paddr));
}
#endif