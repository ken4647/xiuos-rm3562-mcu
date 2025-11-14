/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file boot.S
*
* This file contains the initial startup code for the Cortex A9 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a ecm/sdm 10/20/09 Initial version
* 3.06a sgd     05/15/12 Updated L2CC Auxiliary and Tag RAM Latency control
*			 register settings.
* 3.06a asa 	06/17/12 Modified the TTBR settings and L2 Cache auxiliary
*		         register settings.
* 3.07a asa     07/16/12 Modified the L2 Cache controller settings to improve
*			 performance. Changed the property of the ".boot"
*			 section.
* 3.07a sgd     08/21/12 Modified the L2 Cache controller and cp15 Aux Control
*               Register settings
* 3.09a sgd     02/06/13 Updated SLCR l2c Ram Control register to a
*               value of 0x00020202. Fix for CR 697094 (SI#687034).
* 3.10a srt     04/18/13 Implemented ARM Erratas. Please refer to file
*			 'xil_errata.h' for errata description
* 4.2   pkp	06/19/14 Enabled asynchronous abort exception
* 5.0	pkp	16/15/14 Modified initialization code to enable scu after
*			 MMU is enabled
* 5.1   pkp	05/13/15 Changed the initialization order so to first invalidate
*			 caches and TLB, enable MMU and caches, then enable SMP
*			 bit in ACTLR. L2Cache invalidation and enabling of L2Cache
*			 is done later.
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#include "xparameters.h"
#include "xil_errata.h"

.globl MMUTable
.global _boot_start
.global stacks_start
.global _vector_jumper



.set SYS_stack,		stacks_start
.set vector_base,	_vector_jumper
.set FPEXC_EN,		0x40000000		/* FPU enable bit, (1 << 30) */

/* this initializes the various processor modes */
_boot_start:
#if XPAR_CPU_ID==0
/* only allow cpu0 through */
	mrc	p15,0,r1,c0,c0,5
	and	r1, r1, #0xf
	cmp	r1, #0
	beq	OKToRun
EndlessLoop0:
	wfe
	b	EndlessLoop0

#elif XPAR_CPU_ID==1
/* only allow cpu1 through */
	mrc	p15,0,r1,c0,c0,5
	and	r1, r1, #0xf
	cmp	r1, #1
	beq	OKToRun
EndlessLoop1:
	wfe
	b	EndlessLoop1
#endif

OKToRun:
	mrc     p15, 0, r0, c0, c0, 0		/* Get the revision */
	and     r5, r0, #0x00f00000
	and     r6, r0, #0x0000000f
	orr     r6, r6, r5, lsr #20-4

#ifdef CONFIG_ARM_ERRATA_742230
        cmp     r6, #0x22                       /* only present up to r2p2 */
        mrcle   p15, 0, r10, c15, c0, 1         /* read diagnostic register */
        orrle   r10, r10, #1 << 4               /* set bit #4 */
        mcrle   p15, 0, r10, c15, c0, 1         /* write diagnostic register */
#endif

#ifdef CONFIG_ARM_ERRATA_743622
	teq     r5, #0x00200000                 /* only present in r2p* */
	mrceq   p15, 0, r10, c15, c0, 1         /* read diagnostic register */
	orreq   r10, r10, #1 << 6               /* set bit #6 */
	mcreq   p15, 0, r10, c15, c0, 1         /* write diagnostic register */
#endif

	/* set VBAR to the _vector_table address in linker script */
	ldr	r0, =vector_base
	mcr	p15, 0, r0, c12, c0, 0

	/*invalidate scu*/
	ldr	r7, =0xf8f0000c
	ldr	r6, =0xffff
	str	r6, [r7]

	/* Invalidate caches and TLBs */
	mov	r0,#0				/* r0 = 0  */
	mcr	p15, 0, r0, c8, c7, 0		/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */
	mcr	p15, 0, r0, c7, c5, 6		/* Invalidate branch predictor array */
	bl	boot_invalidate_dcache		/* invalidate dcache */

	/* Disable MMU, if enabled */
	mrc	p15, 0, r0, c1, c0, 0		/* read CP15 register 1 */
	bic	r0, r0, #0x1			/* clear bit 0 */
	mcr	p15, 0, r0, c1, c0, 0		/* write value back */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the system stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x13			/* SVC mode */
	msr	cpsr, r2
	ldr	r13,=SYS_stack			/* SYS stack pointer */

	/*set scu enable bit in scu*/
	ldr	r7, =0xf8f00000
	ldr	r0, [r7]
	orr	r0, r0, #0x1
	str	r0, [r7]

	mov	r0, r0
	mrc	p15, 0, r1, c1, c0, 2		/* read cp access control register (CACR) into r1 */
	orr	r1, r1, #(0xf << 20)		/* enable full access for p10 & p11 */
	mcr	p15, 0, r1, c1, c0, 2		/* write back into CACR */

	/* enable vfp */
	fmrx	r1, FPEXC			/* read the exception register */
	orr	r1,r1, #FPEXC_EN		/* set VFP enable bit, leave the others in orig state */
	fmxr	FPEXC, r1			/* write back the exception register */

	mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */
	orr	r0, r0, #(0x01 << 11)		/* #0x8000 */
	mcr	p15,0,r0,c1,c0,0

	mrc	p15,0,r0,c1,c0,1		/* read Auxiliary Control Register */
	orr	r0, r0, #(0x1 << 2)		/* enable Dside prefetch */
	orr	r0, r0, #(0x1 << 1)		/* enable L2 Prefetch hint */
	mcr	p15,0,r0,c1,c0,1		/* write Auxiliary Control Register */

	mrs	r0, cpsr			/* get the current PSR */
	bic	r0, r0, #0x100			/* enable asynchronous abort exception */
	msr	cpsr_xsf, r0


	b	_start				/* jump to C startup code */
	and	r0, r0, r0			/* no op */

.Ldone:	b	.Ldone				/* Paranoia: we should never get here */


/*
 *************************************************************************
 *
 * boot_invalidate_dcache - invalidate the entire d-cache by set/way
 *
 * Note: for Cortex-A9, there is no cp instruction for invalidating
 * the whole D-cache. Need to invalidate each line.
 *
 *************************************************************************
 */
boot_invalidate_dcache:
	mrc	p15, 1, r0, c0, c0, 1		/* read CLIDR */
	ands	r3, r0, #0x7000000
	mov	r3, r3, lsr #23			/* cache level value (naturally aligned) */
	beq	finished
	mov	r10, #0				/* start with level 0 */
loop1:
	add	r2, r10, r10, lsr #1		/* work out 3xcachelevel */
	mov	r1, r0, lsr r2			/* bottom 3 bits are the Cache type for this level */
	and	r1, r1, #7			/* get those 3 bits alone */
	cmp	r1, #2
	blt	skip				/* no cache or only instruction cache at this level */
	mcr	p15, 2, r10, c0, c0, 0		/* write the Cache Size selection register */
	isb					/* isb to sync the change to the CacheSizeID reg */
	mrc	p15, 1, r1, c0, c0, 0		/* reads current Cache Size ID register */
	and	r2, r1, #7			/* extract the line length field */
	add	r2, r2, #4			/* add 4 for the line length offset (log2 16 bytes) */
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		/* r4 is the max number on the way size (right aligned) */
	clz	r5, r4				/* r5 is the bit position of the way size increment */
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		/* r7 is the max number of the index size (right aligned) */
loop2:
	mov	r9, r4				/* r9 working copy of the max way size (right aligned) */
loop3:
	orr	r11, r10, r9, lsl r5		/* factor in the way number and cache number into r11 */
	orr	r11, r11, r7, lsl r2		/* factor in the index number */
	mcr	p15, 0, r11, c7, c6, 2		/* invalidate by set/way */
	subs	r9, r9, #1			/* decrement the way number */
	bge	loop3
	subs	r7, r7, #1			/* decrement the index */
	bge	loop2
skip:
	add	r10, r10, #2			/* increment the cache number */
	cmp	r3, r10
	bgt	loop1

finished:
	mov	r10, #0				/* swith back to cache level 0 */
	mcr	p15, 2, r10, c0, c0, 0		/* select current cache level in cssr */
	dsb
	isb

	bx	lr

.end