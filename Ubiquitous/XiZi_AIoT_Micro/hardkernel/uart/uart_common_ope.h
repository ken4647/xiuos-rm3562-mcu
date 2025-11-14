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
 * @file uart_common_ope.h
 * @brief support uart common operation
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.20
 */

#ifndef UART_COMMON_OPE_H
#define UART_COMMON_OPE_H

#include <stdint.h>

#include "actracer.h"
#include "printf.h"

struct XiziSerialDriver {
    void (*sys_serial_init)();
    uint32_t (*get_serial_irqnum)();

    uint8_t (*getc)();
    void (*putc)(uint8_t);
};

struct XiziSerialDriver* hardkernel_uart_init(struct TraceTag* hardkernel_tag);

int serial_init(struct TraceTag* uart_driver_tag);

#endif
