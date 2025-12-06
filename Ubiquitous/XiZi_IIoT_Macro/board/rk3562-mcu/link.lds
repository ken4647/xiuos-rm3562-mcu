/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

MEMORY
{
    RAM(rxw) : ORIGIN = 0x00000000, LENGTH = 0x100000
    LINUX_RPMSG (rxw) : ORIGIN = 0x00100000, LENGTH = 0x00500000
}

ENTRY(rk3562_entry)

__SYSTEM_STACKSIZE__ = 0x1000;

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))

        _stext = .;
        . = ALIGN(16);
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.text)                        /* remaining code */
        *(.text.*)                      /* remaining code */
        *(.rodata)                      /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t*)

        /* section information for  shell */
        . = ALIGN(4);
        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;
        . = ALIGN(4);

        __isrtbl_idx_start = .;
        KEEP(*(.isrtbl.idx))
        __isrtbl_start = .;
        KEEP(*(.isrtbl))
        __isrtbl_end = .;
        . = ALIGN(4);

        . = ALIGN(16);
        _etext = .;
    } > RAM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > RAM

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > RAM

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        __copy_table_end__ = .;
    } > RAM

    . = ALIGN (4);

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        _sidata = .;
    } > RAM
    __exidx_end = .;

    .data : AT (_sidata)
    {
        PROVIDE(__data_start__ = .);
        _sdata = .;
        *(vtable)
        *(.data)
        *(.data.*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(16);
        *(.sdata)
        *(.sdata.*)
        PROVIDE(__data_end__ = .);
        _edata = .;
    } > RAM

    . = ALIGN(16);

    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    } > RAM

    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    } > RAM

    .linux_share_rpmsg (NOLOAD):
    {
        PROVIDE(__linux_share_rpmsg_start__ = .);
        . += LENGTH(LINUX_RPMSG);
        PROVIDE(__linux_share_rpmsg_end__ = .);
    } > LINUX_RPMSG

    . = ALIGN(16);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        PROVIDE(__bss_start__ = .);
        *(.bss)
        *(.bss.*)
        *(.dynbss)
        *(COMMON)
        . = ALIGN(16);
        PROVIDE(__bss_end__ = .);
        _ebss = .;
    } > RAM
    __bss_end = .;
   PROVIDE(end = .);

   .stack ORIGIN(RAM) + LENGTH(RAM) - __SYSTEM_STACKSIZE__ :
  {
    PROVIDE( _heap_end = . );
    . = __SYSTEM_STACKSIZE__;
    PROVIDE( _sp = . );
  } > RAM
}
