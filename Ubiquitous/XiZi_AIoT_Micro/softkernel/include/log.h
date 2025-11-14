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
 * @file log.h
 * @brief log support
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: log.h
Description: log support
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include "uart_common_ope.h"

#define OUTPUT_LEVLE_LOG 0
#define OUTPUT_LEVLE_DEBUG 1
#define OUTPUT_LEVLE_ERROR 2

#define OUTPUT_LEVLE_TEST 3

#define OUTPUT_LEVLE OUTPUT_LEVLE_DEBUG
// #define OUTPUT_LEVLE OUTPUT_LEVLE_LOG

#if (OUTPUT_LEVLE >= OUTPUT_LEVLE_LOG)
#define LOG_PRINTF(f, args...) \
    KPrintf(f, ##args)
#else
#define LOG_PRINTF(f, args...)
#endif

#define LOG(f, args...)                 \
    LOG_PRINTF("LOG: [%s] ", __func__); \
    LOG_PRINTF(f, ##args)

#if (OUTPUT_LEVLE >= OUTPUT_LEVLE_DEBUG)
#define DEBUG_PRINTF(f, args...) \
    KPrintf(f, ##args)
#else
#define DEBUG_PRINTF(f, args...)
#endif

#if (OUTPUT_LEVLE >= OUTPUT_LEVLE_TEST)
#define RECORD_PRINTF(f, args...) \
    KPrintf(f, ##args)
#else
#define RECORD_PRINTF(f, args...)
#endif

#define DEBUG(f, args...)                   \
    DEBUG_PRINTF("DEBUG: [%s] ", __func__); \
    DEBUG_PRINTF(f, ##args)

#define RECORD(f, args...)                   \
    RECORD_PRINTF("DEBUG: [%s] ", __func__); \
    RECORD_PRINTF(f, ##args)

#define ERROR(f, args...)                           \
    KPrintf("ERROR: [%s %d] ", __func__, __LINE__); \
    KPrintf(f, ##args)
