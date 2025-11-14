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
 * @file context_switch.S
 * @brief task context switch functions
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.15
 */

/*************************************************
File name: context_switch.S
Description: task context switch functions
Others: 
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
.global context_switch

context_switch:
	# store original context to stack
	stmfd   r13!, 	{r4-r12, lr}

	# switch the stack
	str     r13, 	[r0]          	// save current sp to the old PCB (**old)
	mov     r13, 	r1            	// load the next stack

	# restore context from stack
	ldmfd   r13!, 	{r4-r12, lr} 

	# return to the caller
	bx      lr
