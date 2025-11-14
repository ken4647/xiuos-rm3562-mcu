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

#pragma once
#include "actracer.h"

struct MmuDriverRightGroup {
    struct TraceTag icache_driver_tag;
    struct TraceTag dcache_driver_tag;
    struct TraceTag intr_driver_tag;
};

struct MmuCommonDone {
    void (*MmuDevPteAttr)(uintptr_t* attr);
    void (*MmuPdeAttr)(uintptr_t* attr);
    void (*MmuUsrPteAttr)(uintptr_t* attr);
    void (*MmuUsrDevPteAttr)(uintptr_t* attr);
    void (*MmuKernPteAttr)(uintptr_t* attr);

    void (*LoadPgdir)(uintptr_t pgdir_paddr);
    void (*TlbFlushAll)();
    void (*TlbFlush)(uintptr_t vaddr, int len);
};

// extern struct MmuCommonDone mmu_common_done;
struct MmuCommonDone* hardkernel_mmu_init(struct TraceTag* hardkernel_tag, char* icache_name, char* dcache_name);