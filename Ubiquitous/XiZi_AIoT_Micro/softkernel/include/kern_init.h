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
 * @file kern_init.h
 * @brief init function header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: kern_init.h
Description: init function header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include "actracer.h"

struct XiziBootNode {
    char* name;
    char* bootinfo;
    bool (*init)();
};

bool hardkernel_init(struct TraceTag*);
bool secondary_cpu_hardkernel_init(int cpu_id, struct TraceTag* _hardkernel_tag);
bool softkernel_init(struct TraceTag* _hardkernel_tag, struct TraceTag* _softkernel_tag);
void show_xizi_bar(void);
