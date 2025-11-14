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
 * @file actracer.h
 * @brief tracer header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */
/*************************************************
File name: actracer.h
Description: tracer header
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

#include "actracer_tag.h"
#include "object_allocator.h"

#define TRACER_NODE_NAME_LEN 32

#define GetSysObject(type, target_tag) (type*)AchieveResource(target_tag)

struct SysTracer {
    TracerNode root_node;
    TraceTag sys_tracer_tag;
    struct slab_allocator node_allocator;
    struct slab_allocator node_name_allocator;
};

void sys_tracer_init();
TraceTag* const RequireRootTag();
bool AchieveResourceTag(struct TraceTag* target, struct TraceTag* owner, char* name);
void* AchieveResource(struct TraceTag* tag);
bool CreateResourceTag(struct TraceTag* new_tag, struct TraceTag* owner, char* name, tracemeta_ac_type type, void* p_resource);
bool DeleteResource(struct TraceTag* target, struct TraceTag* owner);

void debug_list_tracetree();