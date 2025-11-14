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

#include "ccm_pll.h"
#include "gpt.h"
#include "timer.h"

#include "clock_common_op.h"
#include "irq_numbers.h"
#include "multicores.h"

static void _sys_clock_init()
{
    uint32_t freq = get_main_clock(IPG_CLK);

    ccm_init();
    gpt_init(CLKSRC_IPG_CLK, freq / 1000000, RESTART_MODE, WAIT_MODE_EN | STOP_MODE_EN);
    gpt_set_compare_event(kGPTOutputCompare1, OUTPUT_CMP_DISABLE, 1000);
    gpt_counter_enable(kGPTOutputCompare1);
}

static uint32_t _get_clock_int()
{
    return IMX_INT_GPT;
}

static uint64_t _get_tick()
{
    return 0;
}

static uint64_t _get_second()
{
    return 0;
}

static void _clear_clock_intr()
{
    gpt_get_compare_event(kGPTOutputCompare1);
}

static bool _is_timer_expired()
{
    return true;
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