/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
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
 * @file imx_uart.c
 * @brief support imx6q uart function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.09.08
 */

/*************************************************
File name: imx_uart.c
Description: imx6q board uart function
Others:
History:
1. Date: 2023-09-28
Author: AIIT XUOS Lab
Modification:
1. Get init and read/write functions(GetUartClk() and UartInitGpio())
2. Rename read/write char functions to UartPutCharInstance() and UartGetCharInstance() to support multiple uart instances
2. Add UartGetIrqnumInstance() and Global XiziSerialDriver object hardkernel_serial_driver;
*************************************************/
#include "crm_regs.h"
#include "regs_pins.h"
#include "regsuart.h"
#include "soc_memory_map.h"

#include "imx_uart.h"

#include "uart_common_ope.h"

static void UartInitGpio(void)
{
    /* UART1 TXD */
    mxc_iomux_v3_setup_pad(MX6Q_PAD_SD3_DAT6__UART1_TXD);

    /* UART1 RXD */
    mxc_iomux_v3_setup_pad(MX6Q_PAD_SD3_DAT7__UART1_RXD);

    /* UART2 TXD */
    mxc_iomux_v3_setup_pad(MX6Q_PAD_EIM_D26__UART2_TXD);

    /* UART2 RXD */
    mxc_iomux_v3_setup_pad(MX6Q_PAD_EIM_D27__UART2_RXD);
}

static uint32_t GetUartClk(void)
{
    uint32_t freq = 80000000, reg, podf;

    reg = mmio_read(MXC_CCM_CSCDR1);
    podf = (reg & MXC_CCM_CSCDR1_UART_CLK_PODF_MASK) >> MXC_CCM_CSCDR1_UART_CLK_PODF_OFFSET;
    freq /= (podf + 1);

    return freq;
}

static void UartInitRegsInstance(uint32_t instance, uint32_t uart_baud_rate)
{
    uint32_t uart_clk;

    HW_UART_UCR1_WR(instance, 0);
    HW_UART_UCR2_WR(instance, 0);

    while (!(HW_UART_UCR2(instance).B.SRST))
        ;

    HW_UART_UCR3_WR(instance, 0x0704);
    HW_UART_UCR4_WR(instance, 0x8000);
    HW_UART_UESC_WR(instance, 0x002B);
    HW_UART_UTIM_WR(instance, 0);
    HW_UART_UTS_WR(instance, 0);

    uart_clk = GetUartClk();

    HW_UART_UFCR_WR(instance, 4 << 7);
    HW_UART_UBIR_WR(instance, 0x0F);
    HW_UART_UBMR_WR(instance, uart_clk / (2 * uart_baud_rate));

    HW_UART_UCR2_WR(instance, BM_UART_UCR2_WS | BM_UART_UCR2_IRTS | BM_UART_UCR2_RXEN | BM_UART_UCR2_TXEN | BM_UART_UCR2_SRST);
    HW_UART_UCR1_WR(instance, 1);
}

__attribute__((__always_inline__)) static inline void UartPutCharInstance(uint32_t instance, uint8_t* ch)
{
    /* Wait for Tx FIFO not full */
    while (HW_UART_UTS(instance).B.TXFULL)
        ;

    if ((char)*ch == '\n') {
        HW_UART_UTXD_WR(instance, '\r');
        while (HW_UART_UTS(instance).B.TXFULL)
            ;
    }
    HW_UART_UTXD_WR(instance, *ch);
}

__attribute__((__always_inline__)) static inline uint8_t UartGetCharInstance(uint32_t instance)
{
    uint32_t read_data;

    /* If Rx FIFO has no data ready */
    if (!(HW_UART_USR2(instance).B.RDR))
        return 0xFF;

    read_data = HW_UART_URXD_RD(instance);

    /* If error are detected */
    if (read_data & 0x7C00)
        return 0xFF;

    return (uint8_t)read_data;
}

__attribute__((__always_inline__)) static inline uint32_t UartGetIrqnumInstance(uint32_t instance)
{
    return UART_IRQS(instance);
}

static inline void UartInitInstance(uint32_t instance)
{
    static char init = 0;
    if (init == 0) {
        mxc_iomux_v3_init((void*)IOMUXC_BASE_ADDR);
        UartInitGpio();
    }
    init = 1;
    UartInitRegsInstance(instance, UART_BAUD_RATE);
}

static void UartPutChar(uint8_t ch)
{
    UartPutCharInstance(IMX6Q_UART_INSTANCE, &ch);
}

static uint8_t UartGetChar()
{
    return UartGetCharInstance(IMX6Q_UART_INSTANCE);
}

static void UartInit()
{
    UartInitInstance(IMX6Q_UART_INSTANCE);
}

static uint32_t UartGetIrqnum()
{
    return UartGetIrqnumInstance(IMX6Q_UART_INSTANCE);
}

/// @todo make this static
static struct XiziSerialDriver hardkernel_serial_driver = {
    .sys_serial_init = UartInit,
    .get_serial_irqnum = UartGetIrqnum,
    .putc = UartPutChar,
    .getc = UartGetChar,
};

struct XiziSerialDriver* hardkernel_uart_init(struct TraceTag* hardkernel_tag)
{
    hardkernel_serial_driver.sys_serial_init();
    return &hardkernel_serial_driver;
}