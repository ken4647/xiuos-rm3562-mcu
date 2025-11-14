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
#include "actracer.h"
#include "core.h"

#include "clock_common_op.h"
#include "clint.h"

//TODO:
static void enable_timer()
{
    ;
}

static void disable_timer()
{
    ;
}

static void reload_timer()
{
    // interval 1ms
    ;
}


void _sys_clock_init()
{
    clint_timer_init();
    disable_timer();
    reload_timer();
    enable_timer();
}

static uint32_t _get_clock_int()
{
    return 30;
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
    return true;
}

static void _clear_clock_intr()
{
    disable_timer();
    reload_timer();
    enable_timer();
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