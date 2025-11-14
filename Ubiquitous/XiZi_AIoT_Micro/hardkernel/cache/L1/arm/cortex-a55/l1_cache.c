/**
 * @file:    l1_cache.c
 * @brief:   the general management of L1 cache
 * @version: 1.0
 * @author:  AIIT XUOS Lab
 * @date:    2024/04/23
 *
 */

/*************************************************
File name: l1_cache.c
Description: the general management of L1 cache
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. implement the l1 cache operations
2. function names are modified to apply softkernel developement
3. function implementations are from modifications of imx6 SDK package
*************************************************/

#include "l1_cache.h"
extern void __asm_flush_dcache_all();
extern void __asm_flush_l3_dcache();
extern void __asm_invalidate_icache_all();
extern void __asm_invalidate_l3_icache();

void InvalidateL1Dcache(uintptr_t start, uintptr_t end)
{
    uint64_t length = end - start;
    uint64_t addr = start;
    uint64_t ccsidr_el1;
    uint64_t line_size;
    uint64_t va;
    // get the cache line size

    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1));
    line_size = 1 << ((ccsidr_el1 & 0x7) + 4);

    // align the address with line
    const uintptr_t end_addr = (const uintptr_t)((uint64_t)addr + length);

    do {
        va = (uint64_t)((uint64_t)addr & (~(line_size - 1)));

        // Invalidate data cache line to PoC (Point of Coherence) by va.
        __asm__ __volatile__("dc ivac, %0 " : : "r"(va));

        // increment addres to next line and decrement lenght
        addr = (uintptr_t)((uint64_t)addr + line_size);
    } while (addr < end_addr);

    // All Cache, Branch predictor and TLB maintenance operations before followed instruction complete
    DSB();
}

void InvalidateL1DcacheAll(void)
{
    uint64_t ccsidr_el1; // Cache Size ID
    int num_sets; // number of sets
    int num_ways; // number of ways
    uint32_t wayset; // wayset parameter

    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1)); // Read Cache Size ID

    // Fill number of sets  and number of ways from ccsidr_el1 register
    num_sets = ((ccsidr_el1 >> 32) & 0x7FFF) + 1;
    num_ways = ((ccsidr_el1 >> 0) & 0x7FFF) + 1;

    // Invalidation all lines (all Sets in all ways)
    for (int way = 0; way < num_ways; way++) {
        for (int set = 0; set < num_sets; set++) {
            wayset = (way << 30) | (set << 5);
            __asm__ __volatile__("dc isw, %0" : : "r"(wayset));
        }
    }

    // All Cache, Branch predictor and TLB maintenance operations before followed instruction complete
    DSB();
}

void CleanL1Dcache(uintptr_t start, uintptr_t end)
{
    void* addr = (void*)start;
    uintptr_t length = end - start;
    const void* end_addr = (const void*)((uint64_t)addr + length);
    uint64_t ccsidr_el1;
    uint64_t line_size;
    uint64_t va;

    // get the cache line size
    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1));
    line_size = 1 << ((ccsidr_el1 & 0x7) + 4);

    do {
        va = (uint64_t)addr & (~(line_size - 1));
        // Clean data cache line to PoC (Point of Coherence) by va.
        __asm__ __volatile__("dc cvac, %0" : : "r"(va));

        // increment addres to next line and decrement lenght
        addr = (void*)((uint64_t)addr + line_size);
    } while (addr < end_addr);

    // All Cache, Branch predictor and TLB maintenance operations before followed instruction complete
    DSB();
}

void CleanL1DcacheAll(void)
{
    uint64_t ccsidr_el1; // Cache Size ID
    int num_sets; // number of sets
    int num_ways; // number of ways
    uint32_t wayset; // wayset parameter

    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1)); // Read Cache Size ID

    // Fill number of sets  and number of ways from ccsidr_el1 register  This walues are decremented by 1
    num_sets = ((ccsidr_el1 >> 32) & 0x7FFF) + 1;
    num_ways = ((ccsidr_el1 >> 0) & 0x7FFF) + 1;

    // clean all lines (all Sets in all ways)
    for (int way = 0; way < num_ways; way++) {
        for (int set = 0; set < num_sets; set++) {
            wayset = (way << 30) | (set << 5);
            __asm__ __volatile__("dc csw, %0" : : "r"(wayset));
        }
    }

    // All Cache, Branch predictor and TLB maintenance operations before followed instruction complete
    DSB();
}

void FlushL1Dcache(uintptr_t start, uintptr_t end)
{
    void* addr = (void*)start;
    // size_t length=end-start;
    uint64_t va;
    uint64_t ccsidr_el1 = 0, line_size = 0;
    const void* end_addr = (const void*)((uint64_t)end);

    // get the cache line size
    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1));
    line_size = 1 << ((ccsidr_el1 & 0x7) + 4);

    do {
        // Clean data cache line to PoC (Point of Coherence) by va.
        va = (uint64_t)((uint64_t)addr & (~(line_size - 1))); // addr & va_VIRTUAL_ADDRESS_MASK
        __asm__ __volatile__("dc civac, %0" : : "r"(va));

        // increment addres to next line and decrement lenght
        addr = (void*)((uint64_t)addr + line_size);
    } while (addr < end_addr);

    // All Cache, Branch predictor and TLB maintenance operations before followed instruction complete
    DSB();
}

void FlushL1DcacheAll(void)
{
    __asm_flush_dcache_all();
    __asm_flush_l3_dcache();
}

void InvalidateL1IcacheAll()
{
    __asm_invalidate_icache_all();
    __asm_invalidate_l3_icache();
}

void InvalidateL1Icache(uintptr_t start, uintptr_t end)
{
    // uintptr_t length = end - start;
    uintptr_t addr = start;
    uint64_t ccsidr_el1;
    uint64_t line_size;
    uint64_t va;
    const uintptr_t end_addr = (const uintptr_t)((uint64_t)end);
    // get the cache line size
    __asm__ __volatile__("mrs %0, ccsidr_el1" : "=r"(ccsidr_el1));
    line_size = 1 << ((ccsidr_el1 & 0x7) + 4);

    do {
        va = (uint64_t)((uint64_t)addr & (~(line_size - 1)));

        // Invalidate data cache line to PoC (Point of Coherence) by va.
        __asm__ __volatile__("ic ivau, %0 " : : "r"(va));
        // increment addres to next line and decrement lenght
        addr = (uintptr_t)((uint64_t)addr + line_size);
    } while (addr < end_addr);

    // synchronize context on this processor
    ISB();
}

void EnableL1Dcache()
{
    uint64_t sctlr_el1; // System Control Register

    // read sctlr_el1
    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r"(sctlr_el1));

    if (!(sctlr_el1 & SCTLR_EL1_DCACHE_ENABLE)) {
        // set  C bit (data caching enable)
        sctlr_el1 |= SCTLR_EL1_DCACHE_ENABLE;

        // write modified sctlr_el1
        __asm__ __volatile__("msr sctlr_el1, %0" : : "r"(sctlr_el1));

        // data synchronization barrier
        DSB();
    }
}

void DisableL1Dcache()
{
    uint64_t sctlr_el1; // System Control Register

    // read sctlr_el1
    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r"(sctlr_el1));

    // set  C bit (data caching enable)
    sctlr_el1 &= ~SCTLR_EL1_DCACHE_ENABLE;

    // write modified sctlr_el1
    __asm__ __volatile__("msr sctlr_el1, %0" : "=r"(sctlr_el1));

    // data synchronization barrier
    DSB();
}

void EnableL1Icache()
{
    uint64_t sctlr_el1; // System Control Register

    // read sctlr_el1
    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r"(sctlr_el1));

    if (!(sctlr_el1 & SCTLR_EL1_ICACHE_ENABLE)) {
        // set  I bit (data caching enable)
        sctlr_el1 |= SCTLR_EL1_ICACHE_ENABLE;

        // write modified sctlr_el1
        __asm__ __volatile__("msr sctlr_el1, %0" : "=r"(sctlr_el1));

        // Instruction synchronization barrier
        ISB();
    }
}

void DisableL1Icache()
{
    uint64_t sctlr_el1; // System Control Register

    // read sctlr_el1
    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r"(sctlr_el1));

    // set  I bit (data caching enable)
    sctlr_el1 &= ~SCTLR_EL1_ICACHE_ENABLE;

    // write modified sctlr_el1
    __asm__ __volatile__("msr sctlr_el1, %0" : : "r"(sctlr_el1));

    // Instruction synchronization barrier
    ISB();
}