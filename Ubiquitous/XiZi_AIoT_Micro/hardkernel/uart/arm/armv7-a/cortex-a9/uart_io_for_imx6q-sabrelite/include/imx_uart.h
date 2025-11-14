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
 * @file imx_uart.h
 * @brief support imx6q uart function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.09.08
 */

/*************************************************
File name: imx_uart.h
Description: imx6q board uart function
Others:
History:
1. Date: 2023-09-08
Author: AIIT XUOS Lab
Modification:
1. No modifications
*************************************************/

#ifndef __IMX_UART_H__
#define __IMX_UART_H__

#include <stdbool.h>
#include <stdint.h>

#include <irq_numbers.h>

//! @addtogroup diag_uart
//! @{
// clang-format off
//////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////

/* UART specific defines */
#define IMX6Q_UART_INSTANCE     HW_UART2
#define UART_BAUD_RATE          115200
#define IOMUXC_GPR1_OFFSET	0x4

#define UART_IRQS(x) ((x) == HW_UART1 ? IMX_INT_UART1 : (x) == HW_UART2 ? IMX_INT_UART2 \
        : (x) == HW_UART3                                               ? IMX_INT_UART3 \
        : (x) == HW_UART4                                               ? IMX_INT_UART4 \
        : (x) == HW_UART5                                               ? IMX_INT_UART5 \
                                                                        : 0xFFFFFFFF)

// clang-format on
#endif //__IMX_UART_H__
