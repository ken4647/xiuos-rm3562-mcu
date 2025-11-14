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
 * @file start_boot.S
 * @brief boot function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.15
 */

/*************************************************
File name: start_boot.S
Description: cortex-a9 start boot function
Others: 
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "core.h"

.global _boot_start
.global CpuInitCrit
.global primary_cpu_init

_boot_start:
    @ save r0 for cores 1-3, r0 arg field passed by ROM
    @ r0 is a function pointer for secondary cpus
    mov r4, 	r0

	mrs	r0, 	cpsr        /*Enter SVC mode*/
	bic	r0, 	r0, #0x1f
	orr	r0, 	r0, #0xd3
	msr	cpsr,	r0

	bl CpuInitCrit

	/* set NSACR, both Secure and Non-secure access are allowed to NEON */
	MRC 	p15, 0, r0, c1, c1, 2
	ORR 	r0, r0, #(0x3<<10) @ enable fpu/neon
	MCR 	p15, 0, r0, c1, c1, 2
	/* Set the CPACR for access to CP10 and CP11*/
	LDR 	r0, =0xF00000
	MCR 	p15, 0, r0, c1, c0, 2
	/* Set the FPEXC EN bit to enable the FPU */
	MOV 	r3, #0x40000000 
	@VMSR FPEXC, r3
	MCR 	p10, 7, r3, c8, c0, 0

	mrc 	p15, 0, r0, c1, c0, 0
	bic 	r0,  r0, #(0x1<<1) @clear A bit of SCTLR 
	mcr 	p15, 0, r0, c1, c0, 0

    @ clear some registers
    mov     r11, #0
    mov     r12, #0
    mov     lr, #0
    
    ldr		r0, 	=stacks_top  @ symbol defined in linker file
    mov		r1, 	#MODE_STACK_SIZE

    @ get cpu id, and subtract the offset from the stacks base address
    mrc     p15, 	0, r2, c0, c0, 5  @ read multiprocessor affinity register
    and     r2, 	r2, #3        @ mask off, leaving CPU ID field
    mov     r5, 	r2		      @ save cpu id for later
    mul     r3, 	r2, r1
    sub     r0, 	r0, r3
           
    msr     CPSR_c, #ARM_MODE_SVC | I_BIT 
    mov     sp, 	r0
    sub     r0, 	r0, r1 
    
    @ check cpu id - cpu0 is primary cpu
    cmp     r5, 	#0
    beq     primary_cpu_init
    bl      bootmain     @ for secondary cpus, jump to argument function pointer passed in by ROM

    bl      .
    
primary_cpu_init:
   	/* init .bss */
    /* clear the .bss section (zero init) */
    ldr     r1,		=boot_start_addr
    ldr     r2,		=boot_end_addr
    mov     r3,		#0
1:  cmp     r1,		r2
    stmltia r1!,	{r3}
    blt     1b
    
    @ branch to c library entry point 
    mov     r0,		#0 @ argc
    mov     r1,		#0 @ argv
    mov     r2,		#0 @ env
    
	bl bootmain

	.end