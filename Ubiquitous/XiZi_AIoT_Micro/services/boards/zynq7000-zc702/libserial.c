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

/// this file is only used for debug
#include "libserial.h"
#include "usyscall.h"

uint32_t Xil_In32(intptr_t Addr)
{
    return *(volatile uint32_t*)Addr;
}
void Xil_Out32(intptr_t Addr, uint32_t Value)
{
    uint32_t* LocalAddr = (uint32_t*)Addr;
    *LocalAddr = Value;
}

#define XUartPs_IsTransmitFull(BaseAddress) \
    ((*(volatile uint32_t*)((BaseAddress) + 0x002CU) & (uint32_t)0x00000010U) == (uint32_t)0x00000010U)
#define XUartPs_WriteReg(BaseAddress, RegOffset, RegisterValue) \
    Xil_Out32((BaseAddress) + (uint32_t)(RegOffset), (uint32_t)(RegisterValue))

#define XUartPs_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (uint32_t)(RegOffset))
#define XUartPs_IsReceiveData(BaseAddress) \
    !((Xil_In32((BaseAddress) + 0x002CU) & (uint32_t)0x00000002U) == (uint32_t)0x00000002U)

uint8_t XUartPs_RecvByte(uint32_t BaseAddress)
{
    uint32_t RecievedByte;
    /* Wait until there is data */
    while (!XUartPs_IsReceiveData(BaseAddress)) {
        ;
    }
    RecievedByte = XUartPs_ReadReg(BaseAddress, 0x0030U);
    /* Return the byte received */
    return (uint8_t)RecievedByte;
}

#define USER_UART_BASE 0x6FFFF000

bool init_uart_mmio()
{
    static int mapped = 0;
    if (mapped == 0) {
        if (-1 == mmap(USER_UART_BASE, 0xE0001000, 4096, true)) {
            return false;
        }
        mapped = 1;
    }
    return true;
}

void putc(char c)
{
    if (c == '\n') {
        while (XUartPs_IsTransmitFull(USER_UART_BASE)) {
            ;
        }

        XUartPs_WriteReg(USER_UART_BASE, 0x0030U, (uint32_t)'\r');
    }

    while (XUartPs_IsTransmitFull(USER_UART_BASE)) {
        ;
    }

    XUartPs_WriteReg(USER_UART_BASE, 0x0030U, (uint32_t)c);
}

char getc(void)
{
    if (!init_uart_mmio()) {
        return 0xFF;
    }
    return XUartPs_RecvByte(USER_UART_BASE);
}
