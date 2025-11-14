/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY  OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file nxp_imx6q_sabrelite.lds
 * @brief nxp imx6q sabrelite lds function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */ 

OUTPUT_FORMAT("elf32-littlearm")                   /*binary file format*/
OUTPUT_ARCH(arm)                                   /*arch target*/
ENTRY(_vector_jumper)                                 /*entry address pointer*/

/* mx6dq has 256kB of OCRAM
 *      +-------------------+ 0x0093FFFF
 *      |     RAM Vector    |
 *      +-------------------+ 0x0093FFB8
 *      |       Stack       |
 *      |        8KB        | 
 *      +-------------------+ 0x0093E000
 *      |     MMU Table     |
 *      |       24KB        |  
 *      +-------------------+ 0x00938000
 *      |                   |   
 *      |     Free Area     |   
 *      |       195KB       |   
 *      |                   |  
 *      +-------------------+ 0x00907400
 *      |      DCD Table    |  
 *      |        1KB        |  
 *      +-------------------+ 0x00907000
 *      |     Reserved      |
 *      |       28KB        | 
 *      +-------------------+ 0x00900000
 */
/*
#define OCRAM_ORG 0x00907400
#define OCRAM_FREE_SIZE 196K*/

/*
 * External DDR CHIP_MX6DQ/CHIP_MX6SDL start 0x10000000, CHIP_MX6SL start 0x80000000 
 */
/*
#define DDR_ORG 0x10000000
#define DDR_LEN 2048M*/

BOOT_STACK_SIZE = 0x4000;
RAM_VECTORS_SIZE = 72;

/* Specify the memory areas */
/*
	ddr3: physical area: [0x10000000, 0x50000000);
	virt_ddr3: virt area exclude boot(start_sec), that will be [0x90000000 + 0x11000, 0xD0000000)
*/
MEMORY
{
    ocram (rwx)  			: ORIGIN = 0x00900000, LENGTH = 256K
    ddr3 (rwx)   			: ORIGIN = 0x10000000, LENGTH = 1024M
    virt_ddr3 (WRX)   		: ORIGIN = 0x90014000, LENGTH = 1024M
}

SECTIONS
{
	. = ORIGIN(ddr3);
	. = ALIGN(4);

    _start_image_addr = .;

    .ivt :
    {
        . = . + 0x400;
        KEEP(*(.ivt))
    } > ddr3
    
    .boot_data :
    {
        __start_boot_data = .;
        KEEP(*(.boot_data))
    } > ddr3
    
    /* aligned to ease the hexdump read of generated binary */ 
    .dcd_hdr : ALIGN(16)
    {
        KEEP(*(.dcd_hdr))
    } > ddr3

    .dcd_wrt_cmd :
    {
        KEEP(*(.dcd_wrt_cmd))
    } > ddr3

    .dcd_data :
    {
        KEEP(*(.dcd_data))
    } > ddr3

	.start_sec : {
		. = ALIGN(0x1000);

		/* read only area. */
		boot.o(.text)
		imx6q_lowlevel_init.o(.text .text.*)
		bootmmu.o(.text .text.*)

		boot.o(.rodata .rodata.*)
		imx6q_lowlevel_init.o(.rodata .rodata.*)
		bootmmu.o(.rodata .rodata.*)

		/* already initialized to zero. */
		boot.o(.data .data.*)
		imx6q_lowlevel_init.o(.data .data.*)
		bootmmu.o(.data .data.*)

		/* initialization start checkpoint. */
		PROVIDE(boot_start_addr = .);

		boot.o(.bss .bss.* COMMON)
		imx6q_lowlevel_init.o(.bss .bss.* COMMON)
		bootmmu.o(.bss .bss.* COMMON)

		/* stack for booting code. */
		. = ALIGN(0x1000);
		PROVIDE(stacks_start = .);
		. += BOOT_STACK_SIZE;
		PROVIDE(stacks_end = .);
		PROVIDE(stacks_top = .);

		/* initialization end checkpoint. */
		PROVIDE(boot_end_addr = .);
	} > ddr3

	/* Other Kernel code is placed over 0x10011000(phy) and 0x90011000(virt). */
	.text : AT(0x10014000) {
		*(.vectors)
		. = ALIGN(0x1000);
		*(.text .text.* .gnu.linkonce.t.*)
	} > virt_ddr3

	. = ALIGN(0x1000);
	/* Other Kernel code text checkpoint. */
	PROVIDE(kern_text = .);

	.data : {
		*(.data .data.*)
		/* user app */
		. = ALIGN(0x1000);
		PROVIDE(_binary_fs_img_start = .);
		*(.rawdata_fs_img*)
		PROVIDE(_binary_fs_img_end = .);
		PROVIDE(_binary_init_start = .);
		*(.rawdata_init*)
		PROVIDE(_binary_init_end = .);
		PROVIDE(_binary_default_fs_start = .);
		*(.rawdata_memfs*)
		PROVIDE(_binary_default_fs_end = .);
	} > virt_ddr3 

	PROVIDE(kernel_data_begin = .);

	_image_size = . - 0x90000000;

	.bss : {
		PROVIDE(__bss_start__ = .);
		*(.bss .bss.* COMMON)
		PROVIDE(__bss_end__ = .);
	} > virt_ddr3
	. = ALIGN(0x1000);
	PROVIDE(kernel_data_end = .);
}
