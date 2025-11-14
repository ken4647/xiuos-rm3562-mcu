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
 * @file core.c
 * @brief spl boot function
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.04.23
 */

/*************************************************
File name: core.c
Description: cortex-a9 core function, include cpu registers operations、core boot
Others:
History:
1. Date: 2024-04-23
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

/*********cortex-a55 general register************
   EL0            |         EL1        |       EL2      |       EL3

     x0;
     x1;
     x2;
      x3;
     x4;
     x5;
     x6;
     x7;
     x8;
     x9;
     x10;
     x11;
     x12;
     x13;
     x14;
     x15;
     x16;
     x17;
     x18;
     x19;
     x20;
     x21;
     x22;
     x23;
     x24;
     x25;
     x26;
     x27;
     x28;
     x29;
     x30;
*********cortex-a55  special register************
                                                XZR
                                                PC
     SP_EL0             SP_EL1            SP_EL2           SP_EL3
                        SPSR_EL1          SPSR_EL2         SPSR_EL3
                        ELR_EL1           ELR_EL2          ELR_EL3
************************************************/

#include "core.h"