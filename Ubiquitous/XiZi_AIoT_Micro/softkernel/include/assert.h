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
 * @file assert.h
 * @brief provide macro assert
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: assert.h
Description: provide macro assert
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include "log.h"

extern void panic(char*);
#define assert(condition)                       \
    do {                                        \
        if (!(condition)) {                     \
            ERROR("(%s) failed\n", #condition); \
            panic("\n");                        \
        }                                       \
    } while (0)

#define LIKELY(exp) __builtin_expect(exp, 1)
#define UNLIKELY(exp) __builtin_expect(exp, 0)

#define ERROR_FREE
