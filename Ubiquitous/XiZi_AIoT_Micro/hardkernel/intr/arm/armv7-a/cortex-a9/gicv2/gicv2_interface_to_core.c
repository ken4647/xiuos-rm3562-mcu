/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
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
/**
 * @file gicv2_distributer_to_core.c
 * @brief gicv2 operation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: gicv2_distributer_to_core.c
Description: gicv2 operation
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. take only gicc part of functions
*************************************************/
#include "gicv2_common_opa.h"
#include "gicv2_registers.h"

static inline gicc_t* gic_get_gicc(void)
{
    uint32_t base = get_arm_private_peripheral_base() + kGICCBaseOffset;
    return (gicc_t*)base;
}

void gic_cpu_enable(bool enableIt)
{
    gicc_t* gicc = gic_get_gicc();

    if (enableIt) {
        gicc->CTLR |= kBM_GICC_CTLR_EnableS | kBM_GICC_CTLR_EnableNS;
    } else {
        gicc->CTLR &= ~(kBM_GICC_CTLR_EnableS | kBM_GICC_CTLR_EnableNS);
    }
}

void gic_set_cpu_priority_mask(uint32_t priority)
{
    gicc_t* gicc = gic_get_gicc();
    gicc->PMR = priority & 0xff;
}

inline uint32_t gic_read_irq_ack(void)
{
    gicc_t* gicc = gic_get_gicc();
    return gicc->IAR;
}

void gic_write_end_of_irq(uint32_t irqID)
{
    gicc_t* gicc = gic_get_gicc();
    gicc->EOIR = irqID;
}

void gic_init_cpu(void)
{
    // Init the GIC CPU interface.
    gic_set_cpu_priority_mask(0xff);

    // Disable preemption.
    gicc_t* gicc = gic_get_gicc();
    gicc->BPR = 7;

    // Enable signaling the CPU.
    gic_cpu_enable(true);
}