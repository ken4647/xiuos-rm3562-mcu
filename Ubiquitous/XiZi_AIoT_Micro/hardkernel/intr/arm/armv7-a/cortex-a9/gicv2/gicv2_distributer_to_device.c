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
 * @file gicv2_distributer_to_device.c
 * @brief gicv2 operation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: gicv2_distributer_to_device.c
Description: gicv2 operation
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. take only gicd part of functions
*************************************************/
#include "string.h"

#include "gicv2_common_opa.h"
#include "gicv2_registers.h"

static inline gicd_t* gic_get_gicd(void);

static inline gicd_t* gic_get_gicd(void)
{
    uint32_t base = get_arm_private_peripheral_base() + kGICDBaseOffset;
    return (gicd_t*)base;
}

void gic_enable(bool enableIt)
{
    gicd_t* gicd = gic_get_gicd();

    if (enableIt) {
        // Enable both secure and non-secure.
        gicd->CTLR |= kBM_GICD_CTLR_EnableGrp0 | kBM_GICD_CTLR_EnableGrp1;
    } else {
        // Clear the enable bits.
        gicd->CTLR &= ~(kBM_GICD_CTLR_EnableGrp0 | kBM_GICD_CTLR_EnableGrp1);
    }
}

void gic_set_irq_security(uint32_t irqID, bool isSecure)
{
    gicd_t* gicd = gic_get_gicd();

    uint32_t reg = irq_get_register_offset(irqID);
    uint32_t mask = irq_get_bit_mask(irqID);

    uint32_t value = gicd->IGROUPRn[reg];
    if (!isSecure) {
        value &= ~mask;
    } else {
        value |= mask;
    }
    gicd->IGROUPRn[reg] = value;
}

void gic_enable_irq(uint32_t irqID, bool isEnabled)
{
    gicd_t* gicd = gic_get_gicd();

    uint32_t reg = irq_get_register_offset(irqID);
    uint32_t mask = irq_get_bit_mask(irqID);

    // Select set-enable or clear-enable register based on enable flag.
    if (isEnabled) {
        gicd->ISENABLERn[reg] = mask;
    } else {
        gicd->ICENABLERn[reg] = mask;
    }
}

void gic_set_irq_priority(uint32_t ID, uint32_t priority)
{
    gicd_t* gicd = gic_get_gicd();

    // Update the priority register. The priority registers are byte accessible, and the register
    // struct has the priority registers as a byte array, so we can just index directly by the
    // interrupt ID.
    gicd->IPRIORITYRn[ID] = priority & 0xff;
}

void gic_set_cpu_target(uint32_t irqID, unsigned cpuNumber, bool enableIt)
{
    // Make sure the CPU number is valid.
    // assert(cpuNumber <= 7);

    gicd_t* gicd = gic_get_gicd();
    uint8_t cpuMask = 1 << cpuNumber;

    // Like the priority registers, the target registers are byte accessible, and the register
    // struct has the them as a byte array, so we can just index directly by the
    // interrupt ID.
    if (enableIt) {
        gicd->ITARGETSRn[irqID] |= (cpuMask & 0xff);
    } else {
        gicd->ITARGETSRn[irqID] &= ~(cpuMask & 0xff);
    }
}

void gic_send_sgi(uint32_t irqID, uint32_t target_list, uint32_t filter_list)
{
    gicd_t* gicd = gic_get_gicd();

    gicd->SGIR = (filter_list << kBP_GICD_SGIR_TargetListFilter)
        | (target_list << kBP_GICD_SGIR_CPUTargetList)
        | (irqID & 0xf);
}

void gic_init(void)
{
    volatile gicd_t* gicd = gic_get_gicd();

    // First disable the distributor.
    gic_enable(false);

    for (uint32_t i = 0; i < 8; i++) {
        gicd->IGROUPRn[i] = 0;
    }

    for (uint32_t i = 0; i < 255; i++) {
        *(uint32_t*)(&gicd->IPRIORITYRn[i * sizeof(uint32_t)]) = (uint32_t)0x80808080;
        // memset((void*)&gicd->IPRIORITYRn[i * sizeof(uint32_t)], 0x80, sizeof(uint32_t));
        *(uint32_t*)(&gicd->ITARGETSRn[i * sizeof(uint32_t)]) = (uint32_t)0x01010101;
        // memset((void*)&gicd->IPRIORITYRn[i * sizeof(uint32_t)], 0x01, sizeof(uint32_t));
    }

    // Init the GIC CPU interface.
    gic_init_cpu();

    // Now enable the distributor.
    gic_enable(true);
}
