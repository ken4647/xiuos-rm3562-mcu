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
 * @file gicv3.c
 * @brief gicv3 operation
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.05.10
 */

/*************************************************
File name: gicv3.c
Description: gicv3 operation
Others:
History:
Author: AIIT XUOS Lab
Modification:
*************************************************/
#include <string.h>

#include "core.h"
#include "gicv3_common_opa.h"
#include "gicv3_registers.h"

static struct {
    char* gicd;
    char* rdist_addrs[NR_CPU];
} gicv3;

static inline uint32_t icc_igrpen1_el1()
{
    uint32_t x;
    __asm__ volatile("mrs %0, S3_0_C12_C12_7" : "=r"(x));
    return x;
}

static inline void w_icc_igrpen1_el1(uint32_t x)
{
    __asm__ volatile("msr S3_0_C12_C12_7, %0" : : "r"(x));
}

static inline uint32_t icc_pmr_el1()
{
    uint32_t x;
    __asm__ volatile("mrs %0, S3_0_C4_C6_0" : "=r"(x));
    return x;
}

static inline void w_icc_pmr_el1(uint32_t x)
{
    __asm__ volatile("msr S3_0_C4_C6_0, %0" : : "r"(x));
}

inline uint32_t gic_read_irq_ack()
{
    uint32_t x;
    __asm__ volatile("mrs %0, S3_0_C12_C12_0" : "=r"(x));
    return x;
}

inline void
gic_write_end_of_irq(uint32_t x)
{
    __asm__ volatile("msr S3_0_C12_C12_1, %0" : : "r"(x));
}

static inline uint32_t icc_sre_el1()
{
    uint32_t x;
    __asm__ volatile("mrs %0, S3_0_C12_C12_5" : "=r"(x));
    return x;
}

static inline void w_icc_sre_el1(uint32_t x)
{
    __asm__ volatile("msr S3_0_C12_C12_5, %0" : : "r"(x));
}

static void gicd_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t*)(gicv3.gicd + off) = val;
}

static uint32_t gicd_read(uint32_t off)
{
    return *(volatile uint32_t*)(gicv3.gicd + off);
}

static void gicr_write(uint32_t cpuid, uint32_t off, uint32_t val)
{
    *(volatile uint32_t*)(gicv3.rdist_addrs[cpuid] + off) = val;
}

static uint32_t gicr_read(uint32_t cpuid, uint32_t off)
{
    return *(volatile uint32_t*)(gicv3.rdist_addrs[cpuid] + off);
}

static void giccinit()
{
    w_icc_igrpen1_el1(0);
    w_icc_pmr_el1(0xff);
}

static void gicdinit()
{
    gicd_write(D_CTLR, 0);

    uint32_t typer = gicd_read(D_TYPER);
    uint32_t lines = typer & 0x1f;

    for (int i = 0; i < lines; i++)
        gicd_write(D_IGROUPR(i), ~0);
}

static void gicrinit(uint32_t cpuid)
{
    gicr_write(cpuid, R_CTLR, 0);

    w_icc_sre_el1(icc_sre_el1() | 1);

    gicr_write(cpuid, R_IGROUPR0, ~0);
    gicr_write(cpuid, R_IGRPMODR0, 0);

    uint32_t waker = gicr_read(cpuid, R_WAKER);
    gicr_write(cpuid, R_WAKER, waker & ~(1 << 1));
    while (gicr_read(cpuid, R_WAKER) & (1 << 2))
        ;
}

void gic_enable()
{
    gicd_write(D_CTLR, (1 << 1));
    w_icc_igrpen1_el1(1);
}

void gic_init()
{
    gicv3.gicd = (char*)GICV3;
    for (int i = 0; i < NR_CPU; i++) {
        gicv3.rdist_addrs[i] = (char*)(GICV3_REDIST + (i) * 0x20000);
    }

    gicdinit();
}

void gicv3inithart(uint32_t cpu_id)
{
    giccinit();
    gicrinit(cpu_id);

    gic_enable();
}

static void
gic_enable_int(uint32_t intid)
{
    uint32_t is = gicd_read(D_ISENABLER(intid / 32));
    is |= 1 << (intid % 32);
    gicd_write(D_ISENABLER(intid / 32), is);
}

int gic_int_enabled(uint32_t intid)
{
    uint32_t is = gicd_read(D_ISENABLER(intid / 32));
    return is & (1 << (intid % 32));
}

static void
gic_clear_pending(uint32_t intid)
{
    uint32_t ic = gicd_read(D_ICPENDR(intid / 32));
    ic |= 1 << (intid % 32);
    gicd_write(D_ICPENDR(intid / 32), ic);
}

static void
gic_set_prio0(uint32_t intid)
{
    // set priority to 0
    uint32_t p = gicd_read(D_IPRIORITYR(intid / 4));
    p &= ~((uint32_t)0xff << (intid % 4 * 8)); // set prio 0
    gicd_write(D_IPRIORITYR(intid / 4), p);
}

static void gic_set_target(uint32_t intid, uint32_t cpuid)
{
    uint32_t itargetsr = gicd_read(D_ITARGETSR(intid / 4));
    itargetsr &= ~((uint32_t)0xff << (intid % 4 * 8));
    gicd_write(D_ITARGETSR(intid / 4), itargetsr | ((uint32_t)(1 << cpuid) << (intid % 4 * 8)));
}

static void
gicr_enable_int(uint32_t cpuid, uint32_t intid)
{
    uint32_t is = gicr_read(cpuid, R_ISENABLER0);
    is |= 1 << (intid % 32);
    gicr_write(cpuid, R_ISENABLER0, is);
}

static void
gicr_clear_pending(uint32_t cpuid, uint32_t intid)
{
    uint32_t ic = gicr_read(cpuid, R_ICPENDR0);
    ic |= 1 << (intid % 32);
    gicr_write(cpuid, R_ICPENDR0, ic);
}

static void
gicr_set_prio0(uint32_t cpuid, uint32_t intid)
{
    uint32_t p = gicr_read(cpuid, R_IPRIORITYR(intid / 4));
    p &= ~((uint32_t)0xff << (intid % 4 * 8)); // set prio 0
    gicr_write(cpuid, R_IPRIORITYR(intid / 4), p);
}

void gic_setup_ppi(uint32_t cpuid, uint32_t intid)
{
    gicr_set_prio0(cpuid, intid);
    gicr_clear_pending(cpuid, intid);
    gicr_enable_int(cpuid, intid);
}

void gic_setup_spi(uint32_t cpuid, uint32_t intid)
{
    gic_set_prio0(intid);
    gic_set_target(intid, cpuid);
    gic_clear_pending(intid);
    gic_enable_int(intid);
}

// irq from iar
int gic_iar_irq(uint32_t iar)
{
    return iar & 0x3ff;
}

// interrupt acknowledge register:
// ask GIC what interrupt we should serve.
uint32_t gic_iar()
{
    return gic_read_irq_ack();
}

// tell GIC we've served this IRQ.
void gic_eoi(uint32_t iar)
{
    gic_write_end_of_irq(iar);
}
