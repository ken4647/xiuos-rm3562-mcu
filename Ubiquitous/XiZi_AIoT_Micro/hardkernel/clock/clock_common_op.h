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
 * @file clock_common_op.h
 * @brief clock interfaces of hardkernel
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */
/*************************************************
File name: clock_common_op.h
Description: clock interfaces of hardkernel
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include <stdint.h>

#include "actracer.h"

struct XiziClockDriver {
    void (*sys_clock_init)();
    uint32_t (*get_clock_int)();
    bool (*is_timer_expired)();
    void (*clear_clock_intr)();

    uint64_t (*get_tick)();
    uint64_t (*get_second)();
};

struct XiziClockDriver* hardkernel_clock_init(struct TraceTag* hardkernel_tag);

bool clock_intr_handler_init(struct TraceTag* p_clock_driver_tag);
int xizi_clock_handler(int irq, void* context, void* arg);