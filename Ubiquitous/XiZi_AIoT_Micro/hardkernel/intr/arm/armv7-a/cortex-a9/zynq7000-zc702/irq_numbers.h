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
 * @file irq_numbers.c
 * @brief irq numbers
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2024.03.04
 */

/*************************************************
File name: irq_numbers.c
Description: irq numbers
Others:
History:
1. Date: 2024-03-04
Author: AIIT XUOS Lab
Modification:
1. Add HW_NR_IRQS
*************************************************/
#pragma once

#include "xparameters_ps.h"
#include "xscugic_hw.h"

#define HW_NR_IRQS XSCUGIC_MAX_NUM_INTR_INPUTS