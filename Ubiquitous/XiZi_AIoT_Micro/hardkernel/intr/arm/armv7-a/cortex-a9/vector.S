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
 * @file vector_table.S
 * @brief define vector table function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.15
 */

/*************************************************
File name: vector_table.S
Description: cortex-a9 vector table
Others: 
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/


.globl		_vector_jumper
.globl		_vector_start
.globl		_vector_end
.global 	_start

	.section	.vectors, "ax"
	.globl		_vector_jumper

/* These will be relocated to VECTOR_BASE. */
_vector_jumper:
	ldr		pc, .LOCATION_resethandler			/* 0x00: Reset */
	ldr		pc, .LOCATION_undefinedhandler		/* 0x04: Undefined instruction */
	ldr		pc, .LOCATION_svchandler			/* 0x08: Software interrupt */
	ldr		pc, .LOCATION_prefetchaborthandler	/* 0x0c: Prefetch abort */
	ldr		pc, .LOCATION_dataaborthandler		/* 0x10: Data abort */
	ldr		pc, .LOCATION_addrexcptnhandler		/* 0x14: Address exception (reserved) */
	ldr		pc, .LOCATION_irqhandler			/* 0x18: IRQ */
	ldr		pc, .LOCATION_fiqhandler			/* 0x1c: FIQ */

	.globl		_vector_start
_vector_start:

.LOCATION_resethandler:
	.long	_boot_start
.LOCATION_undefinedhandler:
	.long	_boot_start
.LOCATION_svchandler:
	.long	_boot_start
.LOCATION_prefetchaborthandler:
	.long	_boot_start
.LOCATION_dataaborthandler:
	.long	_boot_start
.LOCATION_addrexcptnhandler:
	.long	_boot_start
.LOCATION_irqhandler:
	.long	_boot_start
.LOCATION_fiqhandler:
	.long	_boot_start

	.globl	_vector_end
_vector_end:
	.size	_vector_start, . - _vector_start
	.end