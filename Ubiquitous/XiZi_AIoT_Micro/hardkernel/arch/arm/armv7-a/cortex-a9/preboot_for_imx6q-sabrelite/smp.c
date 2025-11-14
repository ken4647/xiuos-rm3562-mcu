/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file smp.c
 * @brief start multicore
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.03.10
 */

/*************************************************
File name: smp.c
Description:
Others:
History:
1. Date: 2024-03-10
Author: AIIT XUOS Lab
Modification:
1. No modifications
*************************************************/
#include "cortex_a9.h"
#include "regssrc.h"

extern void _boot_start();
void cpu_start_secondary(uint8_t cpu_id)
{
    // Prepare pointers for ROM code. The entry point is always _start, which does some
    // basic preparatory work and then calls the common_cpu_entry function, which itself
    // calls the entry point saved in s_core_info.
    switch (cpu_id) {
    case 1:
        HW_SRC_GPR3_WR((uint32_t)&_boot_start);
        HW_SRC_SCR.B.CORE1_ENABLE = 1;
        break;

    case 2:
        HW_SRC_GPR5_WR((uint32_t)&_boot_start);
        HW_SRC_SCR.B.CORE2_ENABLE = 1;
        break;

    case 3:
        HW_SRC_GPR7_WR((uint32_t)&_boot_start);
        HW_SRC_SCR.B.CORE3_ENABLE = 1;
        break;
    default:
        break;
    }
}

void start_smp_cache_broadcast(int cpu_id)
{
    return;
}