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
 * @file clock.c
 * @brief clock interfaces of hardkernel
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */
/*************************************************
File name: clock.c
Description: clock interfaces of hardkernel
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stdint.h>

#include "xparameters.h"
#include "xscutimer.h"

#include "clock_common_op.h"
#include "log.h"

XScuTimer global_timer;
#define TIMER_LOAD_VALUE 0x514C7

static void _sys_clock_init()
{
    XScuTimer_Config* timer_cfg = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
    if (XScuTimer_CfgInitialize(&global_timer, timer_cfg, timer_cfg->BaseAddr) != XST_SUCCESS || XScuTimer_SelfTest(&global_timer) != XST_SUCCESS) {
        ERROR("Error initializing timer\n");
        return;
    }
    XScuTimer_EnableInterrupt(&global_timer);
    XScuTimer_EnableAutoReload(&global_timer);
    XScuTimer_LoadTimer(&global_timer, TIMER_LOAD_VALUE);
    XScuTimer_Start(&global_timer);
}

static uint32_t _get_clock_int()
{
    return XPAR_PS7_SCUTIMER_0_INTR;
}

static uint64_t _get_tick()
{
    return 0;
}

static uint64_t _get_second()
{
    return 0;
}

static bool _is_timer_expired()
{
    return XScuTimer_IsExpired(&global_timer);
}

static void _clear_clock_intr()
{
    XScuTimer_ClearInterruptStatus(&global_timer);
}

static struct XiziClockDriver hardkernel_clock_driver = {
    .sys_clock_init = _sys_clock_init,
    .get_clock_int = _get_clock_int,
    .get_tick = _get_tick,
    .get_second = _get_second,
    .is_timer_expired = _is_timer_expired,
    .clear_clock_intr = _clear_clock_intr,
};

struct XiziClockDriver* hardkernel_clock_init(struct TraceTag* hardkernel_tag)
{
    hardkernel_clock_driver.sys_clock_init();
    return &hardkernel_clock_driver;
}