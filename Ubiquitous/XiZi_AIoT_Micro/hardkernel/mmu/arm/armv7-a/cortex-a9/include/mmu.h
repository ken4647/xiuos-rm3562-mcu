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
 * @file mmu.h
 * @brief mmu related configure and registers
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */
/*************************************************
File name: mmu.h
Description: mmu related configure and registers
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
#include "page_table_entry.h"

#define PAGE_DIR_COARSE (1 << 0) // page dir entry attributes
#define PAGE_4K (1 << 1) // page table entry descriptor attributes

/*
Access permission properties.
*/
enum AccessPermission {
    AccessPermission_NoAccess = 0,
    AccessPermission_KernelOnly = 1,
    AccessPermission_KernelUser = 3,
};

void GetDevPteAttr(uintptr_t* attr);
void GetUsrPteAttr(uintptr_t* attr);
void GetUsrDevPteAttr(uintptr_t* attr);
void GetKernPteAttr(uintptr_t* attr);
void GetPdeAttr(uintptr_t* attr);

/*
Enable MMU, cache, write buffer, etc.
*/
#define SCTLR_R(val) __asm__ volatile("mrc p15, 0, %0, c1, c0, 0" : "=r"(val))
#define SCTLR_W(val) __asm__ volatile("mcr p15, 0, %0, c1, c0, 0" ::"r"(val))

/*
Read and write mmu pagetable register base addr
*/
#define TTBR0_R(val) __asm__ volatile("mrc p15, 0, %0, c2, c0, 0" : "=r"(val))
#define TTBR0_W(val) __asm__ volatile("mcr p15, 0, %0, c2, c0, 0" ::"r"(val))

/*
TTBCR is used for choosing TTBR0 and TTBR1 as page table register.
When TTBCR is set to 0, TTBR0 is selected by default.
*/
#define TTBCR_R(val) __asm__ volatile("mrc p15, 0, %0, c2, c0, 2" : "=r"(val))
#define TTBCR_W(val) __asm__ volatile("mcr p15, 0, %0, c2, c0, 2" ::"r"(val))

/*
DACR registers are used to control memory privilage.
The 32-bit is divided into 16 fields, each of which can be 0x00(no access), 0x01(client), 0x10(reserved), or 0x11(manager).
The domain value is usually 0x01. The memory privilage will be controled by pte AP/APX
*/
#define DACR_R(val) __asm__ volatile("mrc p15, 0, %0, c3, c0, 0" : "=r"(val))
#define DACR_W(val) __asm__ volatile("mcr p15, 0, %0, c3, c0, 0" ::"r"(val))

/*
Flush TLB when loading a new page table.
@note If nG is not set in the pte attribute, process switching need flush tlb.
*/
#define CLEARTLB(val) __asm__ volatile("mcr p15, 0, %0, c8, c7, 0" ::"r"(val))

/*
When nG is set in the pte attribute, the process is assigned an ASID, which is stored in the lower 8 bits of the CONTEXTIDR register.
When the process switches, the flush TLB is no longer required anymore.
*/
#define CONTEXTIDR_R(val) __asm__ volatile("mrc p15, 0, %0, c13, c0, 1" : "=r"(val))
#define CONTEXTIDR_W(val) __asm__ volatile("mcr p15, 0, %0, c13, c0, 1" ::"r"(val))

#ifndef __ASSEMBLER__
#include <stdint.h>
__attribute__((always_inline)) static inline uint32_t v2p(void* a) { return ((uint32_t)(a)) - KERN_MEM_BASE; }
__attribute__((always_inline)) static inline void* p2v(uint32_t a) { return (void*)((a) + KERN_MEM_BASE); }
#endif
