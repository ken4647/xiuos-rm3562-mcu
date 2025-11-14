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
 * @file hardkerenl_init.c
 * @brief init hardkernel
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: hardkerenl_init.c
Description: init hardkernel
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "kern_init.h"

#include "cache_common_ope.h"
#include "clock_common_op.h"
#include "mmu_common.h"
#include "trap_common.h"
#include "uart_common_ope.h"

#include "assert.h"
#include "pagetable.h"
#include "spinlock.h"

#define KERN_BOOT_DRIVER(n, bi, f) \
    {                              \
        n, bi, f                   \
    }

static struct TraceTag hardkernel_tag;

static bool xizi_uart_init()
{
    struct XiziSerialDriver* p_uart_driver = hardkernel_uart_init(&hardkernel_tag);
    if (p_uart_driver == NULL) {
        ERROR("uart driver init failed\n");
        return false;
    }

    struct TraceTag uart_driver_tag;
    if (!CreateResourceTag(&uart_driver_tag, &hardkernel_tag, "uart-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_uart_driver)) {
        ERROR("Create uart resource failed.\n");
        return false;
    }

    serial_init(&uart_driver_tag);
    return true;
}

static bool xizi_intr_init()
{
    /* init intr driver */
    struct XiziTrapDriver* p_intr_driver = hardkernel_intr_init(&hardkernel_tag);
    if (UNLIKELY(p_intr_driver == NULL)) {
        ERROR("intr driver init failed\n");
        return false;
    }

    struct TraceTag intr_driver_tag = {};
    if (!CreateResourceTag(&intr_driver_tag, &hardkernel_tag, "intr-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_intr_driver)) {
        ERROR("Create intr resource failed.\n");
        return false;
    }

    /* grant rights to intr dispatcher */
    struct IrqDispatcherRightGroup irq_dispatcher_rights = { .intr_driver_tag = intr_driver_tag };
    if (!intr_distributer_init(&irq_dispatcher_rights)) {
        ERROR("init intr distributer failed.\n");
        return false;
    }

    /* grant rights to syscall dispatcher */
    struct SwiDispatcherRightGroup swi_dispatcher_rights = { .intr_driver_tag = intr_driver_tag };
    if (!swi_distributer_init(&swi_dispatcher_rights)) {
        ERROR("init syscall distributer failed.\n");
        return false;
    }

    /* grant rights to spinlock module*/
    if (!module_spinlock_use_intr_init()) {
        ERROR("spinlock init using interrupt failed\n");
        return false;
    }

    return true;
}

static bool xizi_gpt_init()
{
    /* init clock driver and create clock driver tag */
    struct XiziClockDriver* p_clock_driver = hardkernel_clock_init(&hardkernel_tag);
    if (UNLIKELY(p_clock_driver == NULL)) {
        ERROR("clock driver init failed\n");
        return false;
    }

    struct TraceTag clock_driver_tag;
    if (!CreateResourceTag(&clock_driver_tag, &hardkernel_tag, "clock-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_clock_driver)) {
        ERROR("Create clock resource failed.\n");
        return false;
    }

    /* enable clock interrupt */
    if (!clock_intr_handler_init(&clock_driver_tag)) {
        ERROR("clock interrupt init failed.\n");
        return false;
    }
    // get intr driver tag
    struct TraceTag intr_driver_tag;
    if (!AchieveResourceTag(&intr_driver_tag, &hardkernel_tag, "intr-ac-resource")) {
        ERROR("Achieve intr resource failed.\n");
        return false;
    }
    // register clock handler to intr
    struct XiziTrapDriver* p_intr_driver = GetSysObject(struct XiziTrapDriver, &intr_driver_tag);
    p_intr_driver->bind_irq_handler(p_clock_driver->get_clock_int(), xizi_clock_handler);
    p_intr_driver->single_irq_enable(p_clock_driver->get_clock_int(), 0, 0);
    return true;
}

static bool xizi_cache_init()
{
    struct TraceTag icache_driver_tag, dcache_driver_tag;

    struct ICacheDone* p_icache_done = hardkernel_icache_init(&hardkernel_tag);
    if (UNLIKELY(p_icache_done == NULL)) {
        ERROR("icache driver init failed.\n");
        return false;
    }
    if (!CreateResourceTag(&icache_driver_tag, &hardkernel_tag, "icache-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_icache_done)) {
        ERROR("Create icache resource failed.\n");
        return false;
    }

    struct DCacheDone* p_dcache_done = hardkernel_dcache_init(&hardkernel_tag);
    if (UNLIKELY(p_dcache_done == NULL)) {
        ERROR("dcache driver init failed.\n");
        return false;
    }
    if (!CreateResourceTag(&dcache_driver_tag, &hardkernel_tag, "dcache-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_dcache_done)) {
        ERROR("Create dcache resource failed.\n");
        return false;
    }

    return true;
}

static bool xizi_mmu_init()
{
    struct MmuCommonDone* p_mmu_done = hardkernel_mmu_init(&hardkernel_tag, "icache-ac-resource", "dcache-ac-resource");
    if (UNLIKELY(p_mmu_done == NULL)) {
        ERROR("mmu driver init failed\n");
        return false;
    }

    /* create trace for mmu driver */
    struct TraceTag mmu_driver_tag;
    CreateResourceTag(&mmu_driver_tag, &hardkernel_tag, "mmu-ac-resource", TRACER_HARDKERNEL_AC_RESOURCE, p_mmu_done);

    return true;
}

struct XiziBootNode hardkernel_init_array[] = {
    KERN_BOOT_DRIVER("uart", "Xizi uart driver initialized successfully.", xizi_uart_init),
    KERN_BOOT_DRIVER("intr", "Xizi interrupt driver initialized successfully.", xizi_intr_init),
    KERN_BOOT_DRIVER("cache", "Xizi cache driver initialized successfully.", xizi_cache_init),
    KERN_BOOT_DRIVER("mmu", "Xizi mmu driver initialized successfully.", xizi_mmu_init),
    KERN_BOOT_DRIVER("clock", "Xizi clock driver initialized successfully.", xizi_gpt_init),
};

bool hardkernel_init(struct TraceTag* _hardkernel_tag)
{
    hardkernel_tag = *_hardkernel_tag;

    for (uint32_t i = 0; i < sizeof(hardkernel_init_array) / sizeof(struct XiziBootNode); i++) {
        if (!hardkernel_init_array[i].init()) {
            return false;
        }
        LOG_PRINTF(hardkernel_init_array[i].bootinfo);
        LOG_PRINTF("\n");
    }
    return true;
}

bool secondary_cpu_hardkernel_init(int cpu_id, struct TraceTag* _hardkernel_tag)
{
    struct TraceTag init_intr_tag, init_icache_tag, init_dcache_tag, init_clock_tag, init_mmu_tag;
    AchieveResourceTag(&init_intr_tag, _hardkernel_tag, "intr-ac-resource");
    AchieveResourceTag(&init_icache_tag, _hardkernel_tag, "icache-ac-resource");
    AchieveResourceTag(&init_dcache_tag, _hardkernel_tag, "dcache-ac-resource");
    AchieveResourceTag(&init_clock_tag, _hardkernel_tag, "clock-ac-resource");
    AchieveResourceTag(&init_mmu_tag, _hardkernel_tag, "mmu-ac-resource");
    struct XiziTrapDriver* p_intr_driver = (struct XiziTrapDriver*)AchieveResource(&init_intr_tag);
    struct ICacheDone* p_icache_driver = (struct ICacheDone*)AchieveResource(&init_icache_tag);
    struct DCacheDone* p_dcache_driver = (struct DCacheDone*)AchieveResource(&init_dcache_tag);
    struct XiziClockDriver* p_clock_driver = (struct XiziClockDriver*)AchieveResource(&init_clock_tag);

    // secondary cpu init hardwares
    // intr
    p_intr_driver->sys_irq_init(cpu_id);
    p_intr_driver->cpu_irq_disable();
    // cache
    p_icache_driver->enable();
    p_dcache_driver->enable();
    // clock
    p_clock_driver->sys_clock_init();
    p_intr_driver->single_irq_enable(p_clock_driver->get_clock_int(), cpu_id, 0);
    // mmu
    secondary_cpu_load_kern_pgdir(&init_mmu_tag, &init_intr_tag);
    return true;
}