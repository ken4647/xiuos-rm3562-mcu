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
 * @file memlayout.h
 * @brief virtual memory and physical memory layout
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */
/*************************************************
File name: memlayout.h
Description: virtual memory and physical memory layout
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

// Memory layout
// clang-format off

#define ARCH_BIT        32

/* A9 physical memory layout */
#define PHY_MEM_BASE            (0x10000000)
#define PHY_USER_FREEMEM_BASE   (0x30000000)
#define PHY_USER_FREEMEM_TOP    (0x50000000)
#define PHY_MEM_STOP            (0x50000000)

/* PTE-PAGE_SIZE */
#define LEVEL4_PTE_SHIFT    12
#define LEVEL4_PTE_SIZE     (1 << LEVEL4_PTE_SHIFT)

/* PDE-SECTION_SIZE */
#define LEVEL3_PDE_SHIFT    20
#define LEVEL3_PDE_SIZE     (1 << LEVEL3_PDE_SHIFT)

#define NUM_LEVEL3_PDE      (1 << (ARCH_BIT - LEVEL3_PDE_SHIFT)) // how many PTE in a PT
#define NUM_LEVEL4_PTE      (1 << (LEVEL3_PDE_SHIFT - LEVEL4_PTE_SHIFT)) // how many PTE in a PT
#define NUM_TOPLEVEL_PDE    NUM_LEVEL3_PDE

#define PAGE_SIZE           LEVEL4_PTE_SIZE
#define MAX_NR_FREE_PAGES   ((PHY_MEM_STOP - PHY_MEM_BASE) >> LEVEL4_PTE_SHIFT)

/* User memory layout */
#define NR_MAX_TREADS_PER_TASK  64
#define USER_STACK_SIZE     MODE_STACK_SIZE
#define USER_MEM_BASE       (0x00000000)
#define USER_MEM_TOP        DEV_VRTMEM_BASE
#define USER_IPC_SPACE_BASE (0x70000000)
#define USER_IPC_USE_ALLOCATOR_WATERMARK    (0x70010000)
#define USER_IPC_SPACE_TOP  (USER_MEM_TOP - (NR_MAX_TREADS_PER_TASK * USER_STACK_SIZE))

/* Deivce memory layout */
#define DEV_PHYMEM_BASE     (0x00000000)
#define DEV_VRTMEM_BASE     (0x80000000)
#define DEV_MEM_SIZE          (0x10000000)

/* Kernel memory layout */
#define KERN_MEM_BASE       (0x90000000) // First kernel virtual address
#define KERN_OFFSET         (KERN_MEM_BASE - PHY_MEM_BASE)

/* virtual and physical addr translate */
#define V2P(a) ((uint32_t)((uint32_t)(a)-KERN_OFFSET))
#define P2V(a) ((void*)((void*)(a) + KERN_OFFSET))

#define V2P_WO(x) ((x)-KERN_OFFSET) // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERN_OFFSET) // same as V2P, but without casts

// clang-format on
