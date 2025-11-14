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
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.09.07
 */

/*************************************************
File name: core.c
Description: cortex-a9 core function, include cpu registers operations、core boot
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/

/*********cortex-a9 general register************
Applications level view                       System level view
                        | User | Syetem | Hyp  |   Svc    |   Abort   |   Undefined  |  IRQ  |   FIQ 
            r0             r0      r0     r0       r0        r0             r0          r0        r0
            r1             r1      r1     r1       r1        r1             r1          r1        r1
            r2             r2      r2     r2       r2        r2             r2          r2        r2
            r3             r3      r3     r3       r3        r3             r3          r3        r3
            r4             r4      r4     r4       r4        r4             r4          r4        r4
            r5             r5      r5     r5       r5        r5             r5          r5        r5
            r6             r6      r6     r6       r6        r6             r6          r6        r6
            r7             r7      r7     r7       r7        r7             r7          r7        r7
            r8             r8      r8     r8       r8        r8             r8          r8        r8_fiq
            r9             r9      r9     r9       r9        r9             r9          r9        r9_fiq
            r10            r10     r10    r10      r10       r10            r10        r10        r10_fiq
            r11            r11     r11    r11      r11       r11            r11        r11        r11_fiq
            r12            r12     r12    r12      r12       r12            r12        r12        r12_fiq
            SP             SP      SP     SP_hyp   SP_svc    SP_abt         SP_und     SP_irq     SP_fiq
            LR             LR      LR     LR       LR_svc    LR_abt         LR_und     LR_irq     LR_fiq
            PC             PC      PC     PC       PC        PC             PC         PC         PC
            APSR          CPSR     CPSR   CPSR     CPSR      CPSR           CPSR       CPSR       CPSR
                                          SPSR_hyp SPSR_svc  SPSR_abt       SPSR_und   SPSR_irq   SPSR_fiq 
                                          ELR_hyp
************************************************/

/*********cortex-a9 CP15 register***************
CRn  / opc1       /       CRm              /               opc2             /        description
c0     0,1,2              c0-c7                            0,1,2,3,4,5,6,7           ID寄存器相关操作
c1     0,4                c0,c1                            0,1,2,3,4,5,6,7           SCTLR系统控制寄存器等
c2     0,4                c0,c1                            0,1,2                     TTBR0、TTBR1、TTBCR寄存器
c3     0                  c0                               0                         DACR域控制寄存器
c5     0,4                c0,c1                            0,1                       DFSR数据异常状态寄存器、IFSR指令异常状态寄存器
c6     0,4                c0                               0,2,4                     DFAR数据异常地址寄存器、IFAR指令异常地址寄存器        
c7     0,4                c0,c1,c4,c5,c6,c8,c10,c11,c14    0,1,2,3,4,5,6,7           cache操作相关寄存器
c8     0,4                c3,c5,c6,c7                      0,1,2,3                   TLB操作相关寄存器
c9     0,1,2,3,4,5,6,7    c12,c13,c14                      0,1,2,3,4,5,6,7           性能监测寄存器
c10    0,1,2,3,4,5,6,7    c0,c2                            0,1,2,3,4,5,6,7           TLB权限控制寄存器、内存重映射
c11    0,1,2,3,4,5,6,7    c0,c1,c2,c3,c4,c5,c6,c7,c7,c15   0,1,2,3,4,5,6,7           紧密耦合内存TCM的DMA控制寄存器
c12    0,4                c0,c1                            0,1                       安全扩展寄存器
c13    0,4                c0                               0,1,2,3,4                 上下文ID保存寄存器，用于快速上下文切换
c14    0,1,2,3,4,5,6,7    c0-c15                           0,1,2,3,4,5,6,7           定时器控制寄存器
c15    0,1,2,3,4,5,6,7    c0-c15                           0,1,2,3,4,5,6,7           (svc mode only)自定义寄存器

汇编指令格式：
MRC p15, opc1, r0, CRn, CRm, opc2 -------- 将CP15内寄存器CRn的值读到指定通用寄存器r0中
MCR p15, opc1, r0, CRn, CRm, opc2 -------- 将指定通用寄存器r0的值写到CP15内寄存器CRn中
************************************************/


#include "core.h"