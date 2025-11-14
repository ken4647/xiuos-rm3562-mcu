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
 * @file 
 * @brief 
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.12.02
 */

/*************************************************
File name: mmu_init.c
Description:
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "memlayout.h"
#include <asm/pgtable-bits.h>


#define PTRS_PER_PGD NUM_LEVEL2_PDE
#define PTRS_PER_PMD NUM_LEVEL3_PDE
#define PGDIR_SHIFT     LEVEL2_PDE_SHIFT
#define PGDIR_SIZE      (1 << PGDIR_SHIFT)
#define PMD_SHIFT       LEVEL3_PDE_SHIFT
#define PMD_SIZE        (1 << PMD_SHIFT)

#define _PAGE_KERNEL		(_PAGE_READ \
				| _PAGE_WRITE \
				| _PAGE_PRESENT \
				| _PAGE_ACCESSED \
				| _PAGE_DIRTY \
				| _PAGE_GLOBAL)

#define PFN_PD(x, prot)	(((x) << _PAGE_PFN_SHIFT) | (prot))
#define _PD_PFN(x)	((x) >> _PAGE_PFN_SHIFT)

#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((x) << PAGE_SHIFT)

#define pgd_index(a)  (((a) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pmd_index(a)  (((a) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))


extern char _start[];
extern char _end[];


struct kernel_mapping {
	unsigned long virt_addr;
	uintptr_t phys_addr;
	uintptr_t size;
	/* Offset between linear mapping virtual address and kernel load address */
	unsigned long va_pa_offset;
	/* Offset between kernel mapping virtual address and kernel load address */
	unsigned long va_kernel_pa_offset;
	unsigned long va_kernel_xip_pa_offset;
};
typedef uint64_t phys_addr_t;


struct kernel_mapping kernel_map;

uintptr_t trampoline_pg_dir[PTRS_PER_PGD] __attribute__((aligned(PAGE_SIZE)));
uintptr_t early_pg_dir[PTRS_PER_PGD] __attribute__((aligned(PAGE_SIZE)));

static uintptr_t trampoline_pmd[PTRS_PER_PMD] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t early_pmd[PTRS_PER_PMD] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t early_uart_pmd[PTRS_PER_PMD] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t early_pmd_free[((PHY_USER_FREEMEM_BASE - PHY_MEM_BASE) >> PGDIR_SHIFT) + 1][PTRS_PER_PMD] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t early_plic_pmd[PTRS_PER_PMD] __attribute__((aligned(PAGE_SIZE)));


static uintptr_t *get_pmd_virt_early(phys_addr_t pa)
{
	/* Before MMU is enabled */
	return (uintptr_t *)((uintptr_t)pa);
}


static phys_addr_t alloc_pmd_early(uintptr_t va)
{
	return (uintptr_t)early_pmd;
}


static void create_pmd_mapping_early(uintptr_t *pmdp,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, uintptr_t prot)
{
	uintptr_t pmd_idx = pmd_index(va);

	if (sz == PMD_SIZE) {
		if ((pmdp[pmd_idx]) == 0)
			pmdp[pmd_idx] = (PFN_PD(PFN_DOWN(pa), prot));
		return;
	}
}

static void create_pgd_mapping_early(uintptr_t *pgdp,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, uintptr_t prot)
{
	uintptr_t *nextp;
	phys_addr_t next_phys;
	uintptr_t pgd_idx = pgd_index(va);

	if (sz == PGDIR_SIZE) {
		if ((pgdp[pgd_idx]) == 0)
			pgdp[pgd_idx] = (PFN_PD(PFN_DOWN(pa), prot));
		return;
	}

	if ((pgdp[pgd_idx]) == 0) {
		next_phys = alloc_pmd_early(va);
		pgdp[pgd_idx] = (PFN_PD(PFN_DOWN(next_phys), _PAGE_TABLE));
		nextp = get_pmd_virt_early(next_phys);
		memset(nextp, 0, PAGE_SIZE);
	} else {
		next_phys = PFN_PHYS(_PD_PFN((pgdp[pgd_idx])));
		nextp = get_pmd_virt_early(next_phys);
	}

	create_pmd_mapping_early(nextp, va, pa, sz, prot);
}

static void create_kernel_page_table_early(uintptr_t *pgdir, bool early)
{
	uintptr_t va, end_va;

	end_va = kernel_map.virt_addr + kernel_map.size;
	for (va = kernel_map.virt_addr; va < end_va; va += PMD_SIZE) {
		create_pgd_mapping_early(pgdir, va,
				   kernel_map.phys_addr + (va - kernel_map.virt_addr),
				   PMD_SIZE,
				   (_PAGE_KERNEL | _PAGE_EXEC));
	}
}

static void create_kernel_pgd_mapping_free_early(uintptr_t *pgdp,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, uintptr_t prot)
{
	uintptr_t *nextp;
	phys_addr_t next_phys;
	uintptr_t pgd_idx = pgd_index(va);
	uintptr_t start_pgd_idx = pgd_index(kernel_map.virt_addr);

	if ((pgdp[pgd_idx]) == 0) {
		next_phys = (uintptr_t)early_pmd_free[pgd_idx - start_pgd_idx];
		pgdp[pgd_idx] = (PFN_PD(PFN_DOWN(next_phys), _PAGE_TABLE));
		nextp = get_pmd_virt_early(next_phys);
		memset(nextp, 0, PAGE_SIZE);
	} else {
		next_phys = PFN_PHYS(_PD_PFN((pgdp[pgd_idx])));
		nextp = get_pmd_virt_early(next_phys);
	}

	create_pmd_mapping_early(nextp, va, pa, sz, prot);
}

static void create_kernel_page_table_free_early(uintptr_t *pgdir, bool early)
{
	uintptr_t va, end_va;

	end_va = KERN_MEM_BASE + (PHY_USER_FREEMEM_BASE - PHY_MEM_BASE);
	for (va = KERN_MEM_BASE + kernel_map.size; va < end_va; va += PMD_SIZE) {
		create_kernel_pgd_mapping_free_early(pgdir, va,
				   kernel_map.phys_addr + (va - KERN_MEM_BASE),
				   PMD_SIZE,
				   _PAGE_KERNEL);
	}
}

static void create_plic_page_table_early(uintptr_t *pgdir, bool early)
{
	uintptr_t va;
	uintptr_t pa;

	for (va = PLIC_VIRTMEM_BASE; va < PLIC_VIRTMEM_BASE + PLIC_MEM_SIZE; va += PMD_SIZE) {
		pa = va - PLIC_VIRTMEM_BASE + PLIC_PHYMEM_BASE;
		create_pgd_mapping_early(pgdir, va, (uintptr_t)early_plic_pmd, PGDIR_SIZE, _PAGE_TABLE);
		create_pmd_mapping_early(early_plic_pmd, va, pa, PMD_SIZE, _PAGE_KERNEL);
	}
}

/*
 * setup_vm_early() is called from boot.S with MMU-off.
 *
 * Following requirements should be honoured for setup_vm() to work
 * correctly:
 * 1) It should use PC-relative addressing for accessing kernel symbols.
 *    To achieve this we always use GCC cmodel=medany.
 * 2) The compiler instrumentation for FTRACE will not work for setup_vm()
 *    so disable compiler instrumentation when FTRACE is enabled.
 */

void setup_vm_early(void)
{
	kernel_map.virt_addr = KERNEL_LINK_ADDR;
	
	kernel_map.phys_addr = (uintptr_t)(&_start);
	kernel_map.size = (uintptr_t)(&_end) - kernel_map.phys_addr;

	kernel_map.va_pa_offset = KERN_OFFSET - kernel_map.phys_addr;
	kernel_map.va_kernel_pa_offset = kernel_map.virt_addr - kernel_map.phys_addr;

	/* Setup trampoline PGD and PMD */
	create_pgd_mapping_early(trampoline_pg_dir, kernel_map.virt_addr, (uintptr_t)trampoline_pmd, PGDIR_SIZE, _PAGE_TABLE);
	create_pmd_mapping_early(trampoline_pmd, kernel_map.virt_addr, kernel_map.phys_addr, PMD_SIZE, (_PAGE_KERNEL | _PAGE_EXEC));

	/*
	 * Setup early PGD covering entire kernel which will allow
	 * us to reach paging_init(). We map all memory banks later
	 * in setup_vm_final() below.
	 */
	create_kernel_page_table_early(early_pg_dir, true);

	/* Setup uart PGD and PMD */
	create_pgd_mapping_early(early_pg_dir, DEV_VRTMEM_BASE, (uintptr_t)early_uart_pmd, PGDIR_SIZE, _PAGE_TABLE);
	create_pmd_mapping_early(early_uart_pmd, DEV_VRTMEM_BASE, DEV_PHYMEM_BASE, PMD_SIZE, _PAGE_KERNEL);

	/* Setup kernel free PGD and PMD */
	create_kernel_page_table_free_early(early_pg_dir, true);

	/* Setup PLIC PGD and PMD */
	create_plic_page_table_early(early_pg_dir, true);
}
