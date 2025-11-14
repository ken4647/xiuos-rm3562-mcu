/*
 * Copyright (C) 2008 by Sascha Hauer <kernel@pengutronix.de>
 * Copyright (C) 2009 by Jan Weitzel Phytec Messtechnik GmbH,
 *                       <armlinux@phytec.de>
 *
 * Copyright (C) 2004-2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
/**
 * @file iomux_v3.c
 * @brief support imx6q iomux function, reference from u-boot-2009-08/cpu/arm_cortexa8/mx6/iomux-v3.c
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.09.08
 */
/*************************************************
File name: iomux_v3.c
Description: support imx6q iomux function, reference from u-boot-2009-08/cpu/arm_cortexa8/mx6/iomux-v3.c
Others:
History:
1. Date: 2023-11-23
Author: AIIT XUOS Lab
Modification:
1. No modifications
*************************************************/

#include <iomux_v3.h>

#include <regs_pins.h>
#include <soc_memory_map.h>

#include "mmio_access.h"

static void* base;

void mxc_iomux_v3_init(void* iomux_v3_base)
{
    base = iomux_v3_base;
}

/*
 * configures a single pad in the iomuxer
 */
int mxc_iomux_v3_setup_pad(iomux_v3_cfg_t pad)
{
    uint32_t mux_ctrl_ofs = (pad & MUX_CTRL_OFS_MASK) >> MUX_CTRL_OFS_SHIFT;
    uint32_t mux_mode = (pad & MUX_MODE_MASK) >> MUX_MODE_SHIFT;
    uint32_t sel_input_ofs = (pad & MUX_SEL_INPUT_OFS_MASK) >> MUX_SEL_INPUT_OFS_SHIFT;
    uint32_t sel_input = (pad & MUX_SEL_INPUT_MASK) >> MUX_SEL_INPUT_SHIFT;
    uint32_t pad_ctrl_ofs = (pad & MUX_PAD_CTRL_OFS_MASK) >> MUX_PAD_CTRL_OFS_SHIFT;
    uint32_t pad_ctrl = (pad & MUX_PAD_CTRL_MASK) >> MUX_PAD_CTRL_SHIFT;

    if (mux_ctrl_ofs)
        mmio_write((uintptr_t)(base + mux_ctrl_ofs), (uintptr_t)(mux_mode));

    if (sel_input_ofs)
        mmio_write((uintptr_t)(base + sel_input_ofs), (uintptr_t)(sel_input));

    if (!(pad_ctrl & NO_PAD_CTRL) && pad_ctrl_ofs) {
        if (pad_ctrl & PAD_CTL_LVE) {
            /* Set the bit for LVE */
            pad_ctrl |= (1 << PAD_CTL_LVE_OFFSET);
            pad_ctrl &= ~(PAD_CTL_LVE);
        }
        mmio_write((uintptr_t)(base + pad_ctrl_ofs), (uintptr_t)(pad_ctrl));
    }

    return 0;
}
