/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Portions Copyright (c) 2011-2012 ARM Ltd.  All rights reserved.
 */
/**
 * @file hard_spinlock.S
 * @brief spinlock implementation
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.04.11
 */

/*************************************************
File name: hard_spinlock.S
Description: spinlock implementation
Others: 
History:
Author: AIIT XUOS Lab
Modification:
*************************************************/

//    .arch armv8-a
    .section ".text","ax"
    
    .global cpu_get_current

#define UNLOCKED 0xFF
// int spinlock_lock(spinlock_t * lock, uint64_t timeout)
    .global _spinlock_lock
.func _spinlock_lock
_spinlock_lock:
    mv s0, a0
    li a0, 0       # cpu_get_current
    mv a1, a0
    mv a0, s0
    li a2, UNLOCKED
1:
    lr.w a3, (a0)
    bne a3, a2, 1b
    sc.w a4, a1, (a0)
    bnez a4, 1b
    ret
.endfunc 


// void spinlock_unlock(spinlock_t * lock)
    .global _spinlock_unlock
    .func _spinlock_unlock
_spinlock_unlock:
    mv s0, a0
    li a0, 0       # cpu_get_current
    mv a1, a0
    mv a0, s0
    li a2, UNLOCKED
    lw a3, (a0)
    bne a3, a1, 1f
    sw a2, (a0)
    li a0, 0
    ret
1:
    li a0, -1
    ret
.endfunc

.end

