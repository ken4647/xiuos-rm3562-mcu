/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 *
 * This file is based on sample code from ARMv8 ARM.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#define ASM_NL		 ;

#define SYMBOL_NAME(X)		X

// #define SYMBOL_NAME_LABEL(X)	X##:

#define SYMBOL_NAME_LABEL(X)	X:

#ifndef __ALIGN
#define __ALIGN .align		4
#endif

#ifndef __ALIGN_STR
#define __ALIGN_STR		".align 4"
#endif

#define ALIGN			__ALIGN
#define ALIGN_STR		__ALIGN_STR

#define LENTRY(name) \
	ALIGN ASM_NL \
	SYMBOL_NAME_LABEL(name)

#define ENTRY(name) \
	.globl SYMBOL_NAME(name) ASM_NL \
	LENTRY(name)

#define WEAK(name) \
	.weak SYMBOL_NAME(name) ASM_NL \
	LENTRY(name)


#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name STT_FUNC ASM_NL \
	END(name)

#define CR_M		(1 << 0)	/* MMU enable			*/
#define CR_A		(1 << 1)	/* Alignment abort enable	*/
#define CR_C		(1 << 2)	/* Dcache enable		*/
#define CR_SA		(1 << 3)	/* Stack Alignment Check Enable	*/
#define CR_I		(1 << 12)	/* Icache enable		*/
#define CR_WXN		(1 << 19)	/* Write Permision Imply XN	*/
#define CR_EE		(1 << 25)	/* Exception (Big) Endian	*/

.macro switch_el, xreg, el3_label, el2_label, el1_label
	nop
.endm


/*
 * void __asm_dcache_level(level)
 * flush or invalidate one level cache.
 *
 * x0: cache level
 * x1: 0 clean & invalidate, 1 invalidate only
 * x2~x9: clobbered
 */
ENTRY(__asm_dcache_level)
	nop

loop_set:
	nop
loop_way:
	nop
	ret
ENDPROC(__asm_dcache_level)


/*
 * void __asm_flush_dcache_all(int invalidate_only)
 *
 * x0: 0 clean & invalidate, 1 invalidate only
 *
 * flush or invalidate all data cache by SET/WAY.
 */

ENTRY(__asm_dcache_all)
	nop
	ret
ENDPROC(__asm_dcache_all)


ENTRY(__asm_flush_dcache_all)
	j	__asm_dcache_all
ENDPROC(__asm_flush_dcache_all)


ENTRY(__asm_invalidate_dcache_all)
	j	__asm_dcache_all
ENDPROC(__asm_invalidate_dcache_all)


/*
 * void __asm_flush_dcache_range(start, end)
 *
 * clean & invalidate data cache in the range
 *
 * x0: start address
 * x1: end address
 */

ENTRY(__asm_flush_dcache_range)
	nop
	ret
ENDPROC(__asm_flush_dcache_range)

/*
 * void __asm_invalidate_dcache_range(start, end)
 *
 * invalidate data cache in the range
 *
 * x0: start address
 * x1: end address
 */

ENTRY(__asm_invalidate_dcache_range)
	nop
	ret
ENDPROC(__asm_invalidate_dcache_range)


/*
 * void __asm_invalidate_icache_all(void)
 *
 * invalidate all tlb entries.
 */

ENTRY(__asm_invalidate_icache_all)
	nop
	ret
ENDPROC(__asm_invalidate_icache_all)



ENTRY(__asm_invalidate_l3_dcache)
	nop
	ret
ENDPROC(__asm_invalidate_l3_dcache)
	.weak	__asm_invalidate_l3_dcache



ENTRY(__asm_flush_l3_dcache)
	nop
	ret
ENDPROC(__asm_flush_l3_dcache)
	.weak	__asm_flush_l3_dcache



ENTRY(__asm_invalidate_l3_icache)
	nop
	ret
ENDPROC(__asm_invalidate_l3_icache)
	.weak	__asm_invalidate_l3_icache


/*
 * void __asm_switch_ttbr(ulong new_ttbr)
 *
 * Safely switches to a new page table.
 */

ENTRY(__asm_switch_ttbr)
	nop
	ret
ENDPROC(__asm_switch_ttbr)

ENTRY(__asm_invalidate_tlb_all)
	ret
ENDPROC(__asm_invalidate_tlb_all)