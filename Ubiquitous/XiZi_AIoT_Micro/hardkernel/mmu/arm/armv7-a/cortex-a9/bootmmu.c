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
 * @file bootmmu.c
 * @brief build pagetable and enable mmu in boot time
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: bootmmu.c
Description: build pagetable and enable mmu in boot time
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "core.h"
#include "memlayout.h"
#include "mmu.h"

#include "mmio_access.h"

#include <stdint.h>
#include <string.h>

extern uint32_t kernel_data_end[];
extern uint32_t kernel_data_begin[];

#define NR_PDE_ENTRIES 4096
#define L1_TYPE_SEC (2 << 0)
#define L1_SECT_DEV ((0b010) << 12)
#define L1_SECT_AP0 (1 << 10)
uint32_t boot_pgdir[NR_PDE_ENTRIES] __attribute__((aligned(0x4000))) = { 0 };

static void build_boot_pgdir()
{
    // dev mem
    uint32_t dev_mem_end_idx = (DEV_PHYMEM_BASE + DEV_MEM_SIZE) >> LEVEL3_PDE_SHIFT;
    for (uint32_t i = DEV_PHYMEM_BASE >> LEVEL3_PDE_SHIFT; i < dev_mem_end_idx; i++) {
        boot_pgdir[i] = (i << LEVEL3_PDE_SHIFT) | L1_TYPE_SEC | L1_SECT_DEV | L1_SECT_AP0;
        boot_pgdir[MMIO_P2V_WO(i << LEVEL3_PDE_SHIFT) >> LEVEL3_PDE_SHIFT] = (i << LEVEL3_PDE_SHIFT) | L1_TYPE_SEC | L1_SECT_DEV | L1_SECT_AP0;
    }

    // identical mem
    uint32_t idn_mem_start_idx = PHY_MEM_BASE >> LEVEL3_PDE_SHIFT;
    uint32_t idn_mem_end_idx = PHY_MEM_STOP >> LEVEL3_PDE_SHIFT;
    for (uint32_t i = idn_mem_start_idx; i < idn_mem_end_idx; i++) {
        boot_pgdir[i] = i << LEVEL3_PDE_SHIFT | L1_TYPE_SEC | L1_SECT_AP0;
    }

    // kern mem
    uint32_t kern_mem_start_idx = KERN_MEM_BASE >> LEVEL3_PDE_SHIFT;
    uint32_t kern_mem_end_idx = (KERN_MEM_BASE + (PHY_USER_FREEMEM_BASE - PHY_MEM_BASE)) >> LEVEL3_PDE_SHIFT;
    for (uint32_t i = kern_mem_start_idx; i < kern_mem_end_idx; i++) {
        boot_pgdir[i] = V2P(i << LEVEL3_PDE_SHIFT) | L1_TYPE_SEC | L1_SECT_AP0;
    }
}

static void load_boot_pgdir()
{
    uint32_t val;

    DACR_W(0x55555555); // set domain access control as client
    TTBCR_W(0x0);
    TTBR0_W((uint32_t)boot_pgdir);

    // Enable paging using read/modify/write
    SCTLR_R(val);
    val |= 0x3805; // Enable MMU, cache, write buffer, high vector tbl. Disable subpage.
    SCTLR_W(val);

    // flush all TLB
    DSB();
    CLEARTLB(0);
    ISB();
}

extern void main(void);
static bool _bss_inited = false;
void bootmain()
{
    build_boot_pgdir();
    load_boot_pgdir();
    __asm__ __volatile__("add sp, sp, %0" ::"r"(KERN_MEM_BASE - PHY_MEM_BASE));
    if (!_bss_inited) {
        memset(&kernel_data_begin, 0x00, (uint32_t)kernel_data_end - (uint32_t)kernel_data_begin);
        _bss_inited = true;
    }

    main();
}