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
 * @file trap_common.h
 * @brief image vector table configuration for imx6q
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: trap_common.h
Description: trap interface of hardkernel
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "actracer.h"
#include "core.h"
#include "irq_numbers.h"
#include "memlayout.h"

#define NR_IRQS HW_NR_IRQS
#define NR_MODE_STACKS 4

enum SgiFilterType {
    SgiFilter_TargetList = 0,
    SgiFilter_AllOtherCPUs = 1,
    SgiFilter_OnlyThisCPU = 2,
};

typedef int (*irq_handler_t)(int irq, void* context, void* arg);

struct irq_table_entry {
    irq_handler_t handler;
};

struct XiziTrapDriver {
    /* irq number table*/
    struct irq_table_entry sw_irqtbl[NR_IRQS];

    void (*sys_irq_init)(int);
    int (*cur_cpu_id)();

    void (*cpu_irq_enable)();
    void (*cpu_irq_disable)();
    void (*single_irq_enable)(int irq, int cpu, int prio);
    void (*single_irq_disable)(int irq, int cpu);

    uintptr_t* (*switch_hw_irqtbl)(uintptr_t*);
    void (*bind_irq_handler)(int, irq_handler_t);

    /* check if no if interruptable */
    /* code runs before irq handling */
    uint32_t (*hw_before_irq)();
    uint32_t (*hw_cur_int_num)(uint32_t int_info);
    /* code runs after irq handling */
    void (*hw_after_irq)(uint32_t int_info);
};

struct IrqDispatcherRightGroup {
    struct TraceTag intr_driver_tag;
};

struct SwiDispatcherRightGroup {
    struct TraceTag intr_driver_tag;
};

struct XiziTrapDriver* hardkernel_intr_init(struct TraceTag* hardkernel_tag);

static inline int cur_cpuid(void)
{
    return cpu_get_current();
}

void panic(char* s);

bool intr_distributer_init(struct IrqDispatcherRightGroup*);
void intr_irq_dispatch(struct trapframe* tf);
bool swi_distributer_init(struct SwiDispatcherRightGroup*);
void software_irq_dispatch(struct trapframe* tf);

void dabort_reason(struct trapframe* r);
void iabort_reason(struct trapframe* r);