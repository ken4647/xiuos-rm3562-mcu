/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *    http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <asm/csr.h>
#include "memlayout.h"


.section ".text", "ax"
.globl _boot_start


_boot_start:
	mv s0, a0
	call _debug_uart_init_early
	la a0, debug_string_start
	call _debug_uart_printascii
	mv a0, s0

	j _start_kernel


_start_kernel:
	/* Mask all interrupts */
	csrw CSR_IE, zero
	csrw CSR_IP, zero

	/* Clear BSS for flat non-ELF images */
	la a3, __bss_start
	la a4, __bss_stop
	ble a4, a3, clear_bss_done
clear_bss:
	sd zero, (a3)
	add a3, a3, RISCV_SZPTR
	blt a3, a4, clear_bss
clear_bss_done:

	li a0, 1
	la a2, boot_cpu_hartid
	sd a0, (a2)

	la sp, stacks_top

	/* Initialize page tables and relocate to virtual addresses */
	call setup_vm_early

	la a0, early_pg_dir
	call relocate_enable_mmu
	la sp, stacks_top

	call _debug_uart_init

	/* Restore C environment */
	la tp, init_thread_info
	sw zero, 32(tp)

	/* Start the kernel */
	tail main

relocate_enable_mmu:
	/* Relocate return address */
	la a1, kernel_map
	ld a1, 0(a1)
	la a2, _start
	sub a1, a1, a2
	add ra, ra, a1

	/* Point stvec to virtual address of intruction after satp write */
	la a2, 1f
	add a2, a2, a1
	csrw CSR_TVEC, a2

	/* Compute satp for kernel page tables, but don't load it yet */
	srl a2, a0, PAGE_SHIFT
	li a1, SATP_MODE
	or a2, a2, a1

	/*
	 * Load trampoline page directory, which will cause us to trap to
	 * stvec if VA != PA, or simply fall through if VA == PA.  We need a
	 * full fence here because setup_vm() just wrote these PTEs and we need
	 * to ensure the new translations are in use.
	 */
	la a0, trampoline_pg_dir
	srl a0, a0, PAGE_SHIFT
	or a0, a0, a1
	sfence.vma
	csrw CSR_SATP, a0

1:
	/* Set trap vector to spin forever to help debug */
	la a0, .Lsecondary_park
	csrw CSR_TVEC, a0

	/*
	 * Switch to kernel page tables.  A full fence is necessary in order to
	 * avoid using the trampoline translations, which are only correct for
	 * the first superpage.  Fetching the fence is guarnteed to work
	 * because that first superpage is translated the same way.
	 */
	csrw CSR_SATP, a2
	sfence.vma
	ret

.Lsecondary_park:
	/* We lack SMP support or have too many harts, so park this hart */
	wfi
	j .Lsecondary_park


debug_string_start:  .ascii "XiZi jh7110 boot start\n\0"

