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
 * @file jh7110.lds
 * @brief
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.10.10
 */

OUTPUT_ARCH(riscv)
OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv", "elf64-littleriscv")

ENTRY( _boot_start )

MEMORY {
  vir_ddr3 (rwx)      : ORIGIN = (0 - 0x80000000), LENGTH = 1024M
  phy_ddr3 (rwx)      : ORIGIN = 0x40200000, LENGTH = 1024M
}

BOOT_STACK_SIZE = 0x4000;

SECTIONS
{
  . = ORIGIN(vir_ddr3);
  _start = .;
  _boot_start = .;

  .start_sec : {
    _start_image_addr = .;

    boot.o(.text)
    ns16550.o(.text .text.*)
    mmu_init.o(.text .text.*)

    boot.o(.rodata .rodata.*)
    ns16550.o(.rodata .rodata.*)
    mmu_init.o(.rodata .rodata.*)

    boot.o(.data .data.*)
    ns16550.o(.data .data.*)
    mmu_init.o(.data .data.*)

    PROVIDE(boot_start_addr = .);

    boot.o(.bss .bss.* COMMON)
    ns16550.o(.bss .bss.* COMMON)
    mmu_init.o(.bss .bss.* COMMON)
  
    . = ALIGN(0x1000);
    PROVIDE(stacks_start = .);
    . += BOOT_STACK_SIZE;
    PROVIDE(stacks_end = .);
    PROVIDE(stacks_top = .);

    PROVIDE(boot_end_addr = .);
  }

  .text : {
    . = ALIGN(0x1000);
    *(.text .text.*)
  }

  .data : {
    . = ALIGN(0x1000);
    *(.data .data.*)

    . = ALIGN(0x1000);
    PROVIDE(_binary_fs_img_start = .);
    *(.rawdata_fs_img*)
    PROVIDE(_binary_fs_img_end = .);
    . = ALIGN(0x1000);
    PROVIDE(_binary_init_start = .);
    *(.rawdata_init*)
    PROVIDE(_binary_init_end = .);
    . = ALIGN(0x1000);
    PROVIDE(_binary_default_fs_start = .);
    *(.rawdata_memfs*)
    PROVIDE(_binary_default_fs_end = .);
    PROVIDE(__init_array_start = .);
    PROVIDE(__init_array_end = .);
  }

  .sdata : {
    . = ALIGN(0x1000);
    __global_pointer$ = . + 0x800;
    *(.sdata*)
  }

  .bss : {
    . = ALIGN(0x1000);
    PROVIDE(kernel_data_begin = .);
    PROVIDE(__bss_start = .);
    *(.bss .bss.* COMMON)
    . = ALIGN(0x1000);
    PROVIDE(__bss_end = .);
    PROVIDE(kernel_data_end = .);
    __bss_stop = .;
  }

  . = ALIGN(0x1000);
  _image_size = . - _start;

  . = ALIGN((1 << 21));
  _edata = .;
  _end = .;
}
