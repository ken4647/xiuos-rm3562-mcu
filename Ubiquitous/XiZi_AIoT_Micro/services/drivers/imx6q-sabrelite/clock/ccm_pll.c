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
 * @file ccm_pll.c
 * @brief support imx6q soc ccm pll functions
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.09.08
 */
/*************************************************
File name: ccm_pll.c
Description: support imx6q soc ccm pll functions
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. Delete unnecessary functions;
2. Slim clock_gating_config to fit only uart and gpt(clock)
*************************************************/
#include "soc_memory_map.h"
#include <stdbool.h>
#include <stdint.h>

#include "regsccm.h"
#include "regsccmanalog.h"
#include "regsgpc.h"

#include "regsepit.h"
#include "regsgpt.h"

#include "ccm_pll.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

const uint32_t PLL1_OUTPUT = 792000000;
const uint32_t PLL2_OUTPUT[] = { 528000000, 396000000, 352000000, 198000000, 594000000 };
const uint32_t PLL3_OUTPUT[] = { 480000000, 720000000, 540000000, 508235294, 454736842 };
const uint32_t PLL4_OUTPUT = 650000000;
const uint32_t PLL5_OUTPUT = 650000000;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

uint32_t get_main_clock(main_clocks_t clock)
{
    uint32_t ret_val = 0;
    uint32_t pre_periph_clk_sel = HW_CCM_CBCMR.B.PRE_PERIPH_CLK_SEL;

    switch (clock) {
    case CPU_CLK:
        ret_val = PLL1_OUTPUT;
        break;
#if !defined(CHIP_MX6SL)
    case AXI_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.AXI_PODF + 1);
        break;
    case MMDC_CH0_AXI_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.MMDC_CH0_AXI_PODF + 1);
        break;
#endif
    case AHB_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.AHB_PODF + 1);
        break;
    case IPG_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.AHB_PODF + 1) / (HW_CCM_CBCDR.B.IPG_PODF + 1);
        break;
    case IPG_PER_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.AHB_PODF + 1) / (HW_CCM_CBCDR.B.IPG_PODF + 1) / (HW_CCM_CSCMR1.B.PERCLK_PODF + 1);
        break;
#if !defined(CHIP_MX6SL)
    case MMDC_CH1_AXI_CLK:
        ret_val = PLL2_OUTPUT[pre_periph_clk_sel] / (HW_CCM_CBCDR.B.MMDC_CH1_AXI_PODF + 1);
        break;
#endif
    default:
        break;
    }

    return ret_val;
}

uint32_t get_peri_clock(peri_clocks_t clock)
{
    uint32_t ret_val = 0;

    switch (clock) {
    case UART1_MODULE_CLK:
    case UART2_MODULE_CLK:
    case UART3_MODULE_CLK:
    case UART4_MODULE_CLK:
        // UART source clock is a fixed PLL3 / 6
        ret_val = PLL3_OUTPUT[0] / 6 / (HW_CCM_CSCDR1.B.UART_CLK_PODF + 1);
        break;

    // eCSPI clock:
    //     PLL3(480) -> /8 -> CSCDR2[ECSPI_CLK_PODF]
    case SPI_CLK:
        ret_val = PLL3_OUTPUT[0] / 8 / (HW_CCM_CSCDR2.B.ECSPI_CLK_PODF + 1);
        break;

#if !defined(CHIP_MX6SL)
    case RAWNAND_CLK:
        ret_val = PLL3_OUTPUT[0] / (HW_CCM_CS2CDR.B.ENFC_CLK_PRED + 1) / (HW_CCM_CS2CDR.B.ENFC_CLK_PODF + 1);
        break;

    case CAN_CLK:
        // For i.mx6dq/sdl CAN source clock is a fixed PLL3 / 8
        ret_val = PLL3_OUTPUT[0] / 8 / (HW_CCM_CSCMR2.B.CAN_CLK_PODF + 1);
        break;
#endif

    default:
        break;
    }

    return ret_val;
}

void ccm_set_lpm_wakeup_source(uint32_t irq_id, bool doEnable)
{
    uint32_t reg_offset = 0;
    uint32_t bit_offset = 0;
    uint32_t gpc_imr = 0;

    // calculate the offset of the register handling that interrupt ID
    // ID starts at 32, so for instance ID=89 is handled by IMR2 because
    // the integer part of the division is reg_offset = 2
    reg_offset = (irq_id / 32);
    // and the rest of the previous division is used to calculate the bit
    // offset in the register, so for ID=89 this is bit_offset = 25
    bit_offset = irq_id - 32 * reg_offset;

    // get the current value of the corresponding GPC_IMRx register
    gpc_imr = *((volatile uint32_t*)(HW_GPC_IMR1_ADDR + (reg_offset - 1) * 4));

    if (doEnable) {
        // clear the corresponding bit to unmask the interrupt source
        gpc_imr &= ~(1 << bit_offset);
        // write the new mask
        *((volatile uint32_t*)(HW_GPC_IMR1_ADDR + (reg_offset - 1) * 4)) = (gpc_imr);
    } else {
        // set the corresponding bit to mask the interrupt source
        gpc_imr |= (1 << bit_offset);
        // write the new mask
        *((volatile uint32_t*)(HW_GPC_IMR1_ADDR + (reg_offset - 1) * 4)) = (gpc_imr);
    }
}

/*!
 * Set/unset clock gating for a peripheral.
 * @param   ccm_ccgrx Address of the clock gating register: CCM_CCGR1,...
 * @param   cgx_offset Offset of the clock gating field: CG(x).
 * @param   gating_mode Clock gating mode: CLOCK_ON or CLOCK_OFF.
 */
void ccm_ccgr_config(uint32_t ccm_ccgrx, uint32_t cgx_offset, uint32_t gating_mode)
{
    if (gating_mode == CLOCK_ON) {
        *(volatile uint32_t*)(ccm_ccgrx) |= cgx_offset;
    } else {
        *(volatile uint32_t*)(ccm_ccgrx) &= ~cgx_offset;
    }
}

void clock_gating_config(uint32_t base_address, uint32_t gating_mode)
{
    uint32_t ccm_ccgrx = 0;
    uint32_t cgx_offset = 0;

    switch (base_address) {
    case REGS_EPIT1_BASE:
        ccm_ccgrx = HW_CCM_CCGR1_ADDR;
        cgx_offset = CG(6);
        break;
    case REGS_EPIT2_BASE:
        ccm_ccgrx = HW_CCM_CCGR1_ADDR;
        cgx_offset = CG(7);
        break;
    default:
        break;
    }

    // apply changes only if a valid address was found
    if (ccm_ccgrx != 0) {
        ccm_ccgr_config(ccm_ccgrx, cgx_offset, gating_mode);
    }
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
