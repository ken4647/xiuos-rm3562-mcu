/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef _ASM_RISCV_ASM_H
#define _ASM_RISCV_ASM_H

//#define __ASSEMBLY__

#ifdef __ASSEMBLY__
#define __ASM_STR(x)	x
#else
#define __ASM_STR(x)	#x
#endif


#define REG_L		ld
#define REG_S		sd
#define REG_SC		sc.d
#define REG_ASM		.dword
#define SZREG		8
#define LGREG		3


#define RISCV_PTR		.dword
#define RISCV_SZPTR		8
#define RISCV_LGPTR		3


#define RISCV_INT		__ASM_STR(.word)
#define RISCV_SZINT		__ASM_STR(4)
#define RISCV_LGINT		__ASM_STR(2)


#define RISCV_SHORT		__ASM_STR(.half)
#define RISCV_SZSHORT		__ASM_STR(2)
#define RISCV_LGSHORT		__ASM_STR(1)


#endif /* _ASM_RISCV_ASM_H */
