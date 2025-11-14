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
 * @file trampoline.S
 * @brief trap in and out code
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: trampoline.S.c
Description: trap in and out code
Others: 
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "memlayout.h"

#include "core.h"

.globl trap_return 
.globl user_trap_swi_enter
.globl trap_irq_enter
.globl trap_reset_enter
.globl trap_iabort
.globl trap_dabort
.globl trap_undefined_instruction
.globl init_stack

trap_return:
	# restore context from trapframe
	ldm 	r13, 	{sp, lr}^ 
	add     r13, 	r13, #8
	ldmfd   r13!, 	{r14}         	
	ldmfd   r13!, 	{r2}          	
	msr     spsr_cxsf, r2
	ldmfd   r13!,	{r0-r12, pc}^  	// restore context and return

user_trap_swi_enter:
	# save trapframe to swi stack
	cpsid i
    stmfd   sp!, 	{r0-r12, r14}  	// save context
	mrs     r2, 	spsr            // copy spsr to r2
	stmfd   r13!, 	{r2}          	// save r2(spsr) to the stack

	stmfd   r13!, 	{r14}         	// save r14 again to have one uniform trapframe
	stmfd   r13, 	{sp, lr}^      	// save user mode sp and lr
	sub     r13, 	r13, #8

    # call syscall handler
	mov     r0, 	r13    			
    bl		software_irq_dispatch
	b		trap_return

trap_irq_enter:
	# save it on the stack as r14 is banked
	cpsid i
	sub     r14, 	r14, #4       	// r14 (lr) contains the interrupted PC
	stmfd   r13!, 	{r0-r2, r14}   	//
	mrs     r1, 	spsr        	// save spsr_irq
	mov     r0, 	r13          	// save stack stop (r13_irq)
	add     r13,	r13, #16    	// reset the IRQ stack

	# switch to the SVC mode
	mrs     r2, 	cpsr
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, #ARM_MODE_SVC
	msr     cpsr_cxsf, r2

	# build the trap frame
	ldr     r2, 	[r0, #12]    	// read the r14_irq, then save it
	stmfd   r13!, 	{r2}
	stmfd   r13!, 	{r3-r12}   		// r4-r12 are preserved (non-banked)
	ldmfd   r0, 	{r3-r5}     	// copy r0-r2 over from irq stack
	stmfd   r13!, 	{r3-r5}
	stmfd   r13!, 	{r1}         	// save spsr
	stmfd   r13!, 	{lr}          	// save lr_svc

	stmfd   r13, 	{sp, lr}^     	// save user mode sp and lr
	sub     r13, 	r13, #8

	mov     r0, 	r13           	// trapframe as parameters
	bl      intr_irq_dispatch
	b   	trap_return

trap_dabort:
	# save it on the stack as r14 is banked
	cpsid i
	sub     r14, 	r14, #8       	// r14 (lr) contains the interrupted PC
	stmfd   r13!, 	{r0-r2, r14}   	//
	mrs     r1, 	spsr        	// save spsr_irq
	mov     r0, 	r13          	// save stack stop (r13_irq)
	add     r13,	r13, #16    	// reset the IRQ stack

	# switch to the SVC mode
	mrs     r2, 	cpsr
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, #ARM_MODE_SVC
	msr     cpsr_cxsf, r2

	# build the trap frame
	ldr     r2, 	[r0, #12]    	// read the r14_irq, then save it
	stmfd   r13!, 	{r2}
	stmfd   r13!, 	{r3-r12}   		// r4-r12 are preserved (non-banked)
	ldmfd   r0, 	{r3-r5}     	// copy r0-r2 over from irq stack
	stmfd   r13!, 	{r3-r5}
	stmfd   r13!, 	{r1}         	// save spsr
	stmfd   r13!, 	{lr}          	// save lr_svc

	stmfd   r13, 	{sp, lr}^     	// save user mode sp and lr
	sub     r13, 	r13, #8

	mov     r0, 	r13           	// trapframe as parameters
	bl      dabort_handler

trap_iabort:
# save it on the stack as r14 is banked
	cpsid i
	sub     r14, 	r14, #4       	// r14 (lr) contains the interrupted PC
	stmfd   r13!, 	{r0-r2, r14}   	//
	mrs     r1, 	spsr        	// save spsr_irq
	mov     r0, 	r13          	// save stack stop (r13_irq)
	add     r13,	r13, #16    	// reset the IRQ stack

	# switch to the SVC mode
	mrs     r2, 	cpsr
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, #ARM_MODE_SVC
	msr     cpsr_cxsf, r2

	# build the trap frame
	ldr     r2, 	[r0, #12]    	// read the r14_irq, then save it
	stmfd   r13!, 	{r2}
	stmfd   r13!, 	{r3-r12}   		// r4-r12 are preserved (non-banked)
	ldmfd   r0, 	{r3-r5}     	// copy r0-r2 over from irq stack
	stmfd   r13!, 	{r3-r5}
	stmfd   r13!, 	{r1}         	// save spsr
	stmfd   r13!, 	{lr}          	// save lr_svc

	stmfd   r13, 	{sp, lr}^     	// save user mode sp and lr
	sub     r13, 	r13, #8

	mov     r0, 	r13           	// trapframe as parameters
	bl      iabort_handler

trap_undefined_instruction:
	# save it on the stack as r14 is banked
	cpsid i
	sub     r14, 	r14, #4       	// r14 (lr) contains the interrupted PC
	stmfd   r13!, 	{r0-r2, r14}   	//
	mrs     r1, 	spsr        	// save spsr_irq
	mov     r0, 	r13          	// save stack stop (r13_irq)
	add     r13,	r13, #16    	// reset the IRQ stack

	# switch to the SVC mode
	mrs     r2, 	cpsr
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, #ARM_MODE_SVC
	msr     cpsr_cxsf, r2

	# build the trap frame
	ldr     r2, 	[r0, #12]    	// read the r14_irq, then save it
	stmfd   r13!, 	{r2}
	stmfd   r13!, 	{r3-r12}   		// r4-r12 are preserved (non-banked)
	ldmfd   r0, 	{r3-r5}     	// copy r0-r2 over from irq stack
	stmfd   r13!, 	{r3-r5}
	stmfd   r13!, 	{r1}         	// save spsr
	stmfd   r13!, 	{lr}          	// save lr_svc

	stmfd   r13, 	{sp, lr}^     	// save user mode sp and lr
	sub     r13, 	r13, #8

	mov     r0, 	r13           	// trapframe as parameters
	bl      handle_undefined_instruction

init_stack:
	# set the stack for Other mode
	mrs     r2, 	cpsr
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, r0
	msr     cpsr_cxsf, r2
	mov     sp, 	r1

	@ # switch back to the SVC mode
	bic     r2, 	r2, #ARM_CPSR_MODE_MASK
	orr     r2, 	r2, #ARM_MODE_SVC
	msr     cpsr_cxsf, r2
	bx      lr