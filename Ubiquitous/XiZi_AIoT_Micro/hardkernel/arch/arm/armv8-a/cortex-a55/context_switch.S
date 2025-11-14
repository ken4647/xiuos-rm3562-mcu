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
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.4.10
 */

/*************************************************
File name: context_switch.S
Description: task context switch functions
Others: 
History:
*************************************************/
.global context_switch

context_switch:

    mov x9, sp
    mov x10, sp

    sub x9, x9, #16 * 7
    stp x10/*sp*/, x18, [x9, #16 * 0]
    stp x19, x20, [x9, #16 * 1]
    stp x21, x22, [x9, #16 * 2]
    stp x23, x24, [x9, #16 * 3]
    stp x25, x26, [x9, #16 * 4]
    stp x27, x28, [x9, #16 * 5]
    stp x29, x30, [x9, #16 * 6]

    str x9, [x0]
    mov x9, x1

    ldp x10/*sp*/, x18, [x9, #16 * 0]
    ldp x19, x20, [x9, #16 * 1]
    ldp x21, x22, [x9, #16 * 2]
    ldp x23, x24, [x9, #16 * 3]
    ldp x25, x26, [x9, #16 * 4]
    ldp x27, x28, [x9, #16 * 5]
    ldp x29, x30, [x9, #16 * 6]
    add x9, x9, #16 * 7

    mov sp, x9
    
    ret