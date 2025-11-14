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
 * @file gicv3_common_opa.h
 * @brief gicv3 operation
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.05.07
 */
/*************************************************
File name: gicv3_common_opa.h
Description: gicv3 operation
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. Rename file
*************************************************/
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <mmio_access.h>

//! @name Initialization
//@{
//! @brief Init interrupt handling.
//!
//! This function is intended to be called only by the primary CPU init code, so it will
//! only be called once during system bootup.
//!
//! Also inits the current CPU. You don't need to call gic_init_cpu() separately.
//!
//! @post The interrupt distributor and the current CPU interface are enabled. All interrupts
//!     that were pending are cleared, and all interrupts are made secure (group 0).
void gic_init(void);

//! @name GIC Interrupt Distributor Functions
//@{
//! @brief Enable or disable the GIC Distributor.
//!
//! Enables or disables the GIC distributor passing both secure (group 0) and non-secure
//! (group 1) interrupts to the CPU interfaces.
//!
//! @param enableIt Pass true to enable or false to disable.
void gic_enable();

//! @brief Set the security mode for an interrupt.
//!
//! @param irqID The interrupt number.
//! @param isSecure Whether the interrupt is taken to secure mode.
void gic_set_irq_security(uint32_t irqID, bool isSecure);

//! @brief Enable or disable an interrupt.
//!
//! @param irqID The number of the interrupt to control.
//! @param isEnabled Pass true to enable or false to disable.
void gic_enable_irq(uint32_t irqID, bool isEnabled);

//! @brief Set whether a CPU will receive a particular interrupt.
//!
//! @param irqID The interrupt number.
//! @param cpuNumber The CPU number. The first CPU core is 0.
//! @param enableIt Whether to send the interrupt to the specified CPU. Pass true to enable
//!     or false to disable.
void gic_set_cpu_target(uint32_t irqID, unsigned cpuNumber, bool enableIt);

//! @brief Set an interrupt's priority.
//!
//! @param irq_id The interrupt number.
//! @param priority The priority for the interrupt. In the range of 0 through 0xff, with
//!     0 being the highest priority.
void gic_set_irq_priority(uint32_t irq_id, uint32_t priority);

void gic_setup_spi(uint32_t cpuid, uint32_t intid);
void gic_setup_ppi(uint32_t cpuid, uint32_t intid);

void gicv3inithart(uint32_t cpu_id);
//! @brief Send a software generated interrupt to a specific CPU.
//!
//! @param irq_id The interrupt number to send.
//! @param target_list Each bit indicates a CPU to which the interrupt will be forwarded.
//!     Bit 0 is CPU 0, bit 1 is CPU 1, and so on. If the value is 0, then the interrupt
//!     will not be forwarded to any CPUs. This parameter is only used if @a filter_list
//!     is set to #kGicSgiFilter_UseTargetList.
//! @param filter_list One of the enums of the #_gicd_sgi_filter enumeration. The selected
//!     option determines which CPUs the interrupt will be sent to. If the value
//!     is #kGicSgiFilter_UseTargetList, then the @a target_list parameter is used.
void gic_send_sgi(uint32_t irq_id, uint32_t target_list, uint32_t filter_list);
//@}

//! @name GIC CPU Interface Functions
//@{
//! @brief Enable or disable the interface to the GIC for the current CPU.
//!
//! @param enableIt Pass true to enable or false to disable.
void gic_cpu_enable(bool enableIt);

//! @brief Set the mask of which interrupt priorities the CPU will receive.
//!
//! @param priority The lowest priority that will be passed to the current CPU. Pass 0xff to
//!     allow all priority interrupts to signal the CPU.
void gic_set_cpu_priority_mask(uint32_t priority);

//! @brief Acknowledge starting of interrupt handling and get the interrupt number.
//!
//! Normally, this function is called at the beginning of the IRQ handler. It tells the GIC
//! that you are starting to handle an interupt, and returns the number of the interrupt you
//! need to handle. After the interrupt is handled, you should call gic_write_end_of_irq()
//! to signal that the interrupt is completely handled.
//!
//! In some cases, a spurious interrupt might happen. One possibility is if another CPU handles
//! the interrupt. When a spurious interrupt occurs, the end of the interrupt should be indicated
//! but nothing else.
//!
//! @return The number for the highest priority interrupt available for the calling CPU. If
//!     the return value is 1022 or 1023, a spurious interrupt has occurred.
uint32_t gic_read_irq_ack(void);

//! @brief Signal the end of handling an interrupt.
//!
//! @param irq_id The number of the interrupt for which handling has finished.
void gic_write_end_of_irq(uint32_t irq_id);
//@}

//! @}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////