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

    .arch armv8-a
    .section ".text","ax"
    
    .global cpu_get_current

#define UNLOCKED 0xFF
// int spinlock_lock(spinlock_t * lock, uint64_t timeout)
    .global _spinlock_lock
.func _spinlock_lock
_spinlock_lock:
    mov     w2, #1
    sevl
1: 
    wfe

2: 
    ldaxrb  w1, [x0]                     // check if the spinlock is currently unlocked
    cmp     w1, #UNLOCKED
    bne     1b

    mrs     x1, mpidr_el1                // get our CPU ID
    and     x1, x1, #0xFFF
    lsr     x1, x1, #8
    stxrb   w2, w1, [x0]
    cmp     x2, #0
    bne     2b        // check if the write was successful, if the write failed, start over

    dmb    ish                        // Ensure that accesses to shared resource have completed

    mov    x0, #0
    ret
.endfunc 


// void spinlock_unlock(spinlock_t * lock)
    .global _spinlock_unlock
    .func _spinlock_unlock
_spinlock_unlock:
    mrs     x1, mpidr_el1                // get our CPU ID
    and     x1, x1, #0xFFF
    lsr     x1, x1, #8

    ldr     w2, [x0]
    cmp     w1, w2
    bne     1f                                       //doesn't match,jump to 1

    dmb     ish
    mov     w1, #UNLOCKED 
    str     w1, [x0]
    dsb     ish                                 //Ensure that no instructions following the barrier execute until
                                                // all memory accesses prior to the barrier have completed.
    sevl                                          // send event to wake up other cores waiting on spinlock

    mov     x0, #0                               // return success
    ret
1:
    mov     x0, #1                              //doesn't match, so exit with failure
    ret
.endfunc

.end

