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
 * @file libmem.h
 * @brief support malloc and free in userland
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.01.31
 */

/*************************************************
File name: libmem.h
Description: support malloc and free in userland
Others:
History:
1. Date: 2024-01-31
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
