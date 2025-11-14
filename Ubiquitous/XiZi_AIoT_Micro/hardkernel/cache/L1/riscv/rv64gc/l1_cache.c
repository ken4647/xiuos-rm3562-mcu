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
}

void InvalidateL1DcacheAll(void)
{
}

void CleanL1Dcache(uintptr_t start, uintptr_t end)
{
}

void CleanL1DcacheAll(void)
{
}

void FlushL1Dcache(uintptr_t start, uintptr_t end)
{
}

void FlushL1DcacheAll(void)
{

}

void InvalidateL1IcacheAll()
{

}

void InvalidateL1Icache(uintptr_t start, uintptr_t end)
{

}

void EnableL1Dcache()
{

}

void DisableL1Dcache()
{

}

void EnableL1Icache()
{

}

void DisableL1Icache()
{

}