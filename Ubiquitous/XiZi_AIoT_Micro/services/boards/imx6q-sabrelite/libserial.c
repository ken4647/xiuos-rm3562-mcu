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

/// this file is only used for debug
#include "libserial.h"
#include "usyscall.h"

typedef union _hw_uart_uts {
    uint32_t U;
    struct _hw_uart_uts_bitfields {
        unsigned SOFTRST : 1; //!< [0] Software Reset.
        unsigned RESERVED0 : 2; //!< [2:1] Reserved
        unsigned RXFULL : 1; //!< [3] RxFIFO FULL.
        unsigned TXFULL : 1; //!< [4] TxFIFO FULL.
        unsigned RXEMPTY : 1; //!< [5] RxFIFO Empty.
        unsigned TXEMPTY : 1; //!< [6] TxFIFO Empty.
        unsigned RESERVED1 : 2; //!< [8:7] Reserved
        unsigned RXDBG : 1; //!< [9]
        unsigned LOOPIR : 1; //!< [10] Loop TX and RX for IR Test (LOOPIR) .
        unsigned DBGEN : 1; //!< [11]
        unsigned LOOP : 1; //!< [12] Loop TX and RX for Test.
        unsigned FRCPERR : 1; //!< [13] Force Parity Error.
        unsigned RESERVED2 : 18; //!< [31:14] Reserved
    } B;
} hw_uart_uts_t;

typedef union _hw_uart_usr2 {
    uint32_t U;
    struct _hw_uart_usr2_bitfields {
        unsigned RDR : 1; //!< [0] Receive Data Ready -Indicates that at least 1 character is received and written to the RxFIFO.
        unsigned ORE : 1; //!< [1] Overrun Error .
        unsigned BRCD : 1; //!< [2] BREAK Condition Detected .
        unsigned TXDC : 1; //!< [3] Transmitter Complete .
        unsigned RTSF : 1; //!< [4] RTS Edge Triggered Interrupt Flag.
        unsigned DCDIN : 1; //!< [5] Data Carrier Detect Input .
        unsigned DCDDELT : 1; //!< [6] Data Carrier Detect Delta .
        unsigned WAKE : 1; //!< [7] Wake .
        unsigned IRINT : 1; //!< [8] Serial Infrared Interrupt Flag.
        unsigned RIIN : 1; //!< [9] Ring Indicator Input .
        unsigned RIDELT : 1; //!< [10] Ring Indicator Delta .
        unsigned ACST : 1; //!< [11] Autobaud Counter Stopped .
        unsigned IDLE : 1; //!< [12] Idle Condition .
        unsigned DTRF : 1; //!< [13] DTR edge triggered interrupt flag .
        unsigned TXFE : 1; //!< [14] Transmit Buffer FIFO Empty .
        unsigned ADET : 1; //!< [15] Automatic Baud Rate Detect Complete .
        unsigned RESERVED0 : 16; //!< [31:16] Reserved
    } B;
} hw_uart_usr2_t;

typedef union _hw_uart_urxd {
    uint32_t U;
    struct _hw_uart_urxd_bitfields {
        unsigned RX_DATA : 8; //!< [7:0] Received Data .
        unsigned RESERVED0 : 2; //!< [9:8] Reserved
        unsigned PRERR : 1; //!< [10] In RS-485 mode, it holds the ninth data bit (bit [8]) of received 9-bit RS-485 data
        unsigned BRK : 1; //!< [11] BREAK Detect.
        unsigned FRMERR : 1; //!< [12] Frame Error.
        unsigned OVRRUN : 1; //!< [13] Receiver Overrun.
        unsigned ERR : 1; //!< [14] Error Detect.
        unsigned CHARRDY : 1; //!< [15] Character Ready.
        unsigned RESERVED1 : 16; //!< [31:16] Reserved
    } B;
} hw_uart_urxd_t;

typedef union _hw_uart_utxd {
    uint32_t U;
    struct _hw_uart_utxd_bitfields {
        unsigned TX_DATA : 8; //!< [7:0] Transmit Data .
        unsigned RESERVED0 : 24; //!< [31:8] Reserved.
    } B;
} hw_uart_utxd_t;

#define USER_UART_BASE 0x6FFFF000
// #define USER_UART_BASE 0x621e8000

bool init_uart_mmio()
{
    static int mapped = 0;
    if (mapped == 0) {
        if (-1 == mmap(USER_UART_BASE, 0x021e8000, 4096, true)) {
            return false;
        }
        mapped = 1;
    }
    return true;
}

void putc(char c)
{
    while ((*(volatile hw_uart_uts_t*)(USER_UART_BASE + 0xb4)).B.TXFULL)
        ;

    if (c == '\n') {
        ((*(volatile hw_uart_utxd_t*)(USER_UART_BASE + 0x40)).U = ('\r'));
        while ((*(volatile hw_uart_uts_t*)(USER_UART_BASE + 0xb4)).B.TXFULL)
            ;
    }
    ((*(volatile hw_uart_utxd_t*)(USER_UART_BASE + 0x40)).U = (c));
}

char getc(void)
{
    if (!init_uart_mmio()) {
        return 0xFF;
    }
    uint32_t read_data;

    /* If Rx FIFO has no data ready */
    if (!(*(volatile hw_uart_usr2_t*)(USER_UART_BASE + 0x98)).B.RDR) {
        return 0xFF;
    }

    read_data = ((*(volatile hw_uart_urxd_t*)(USER_UART_BASE + 0x0)).U);

    /* If error are detected */
    if (read_data & 0x7C00)
        return 0xFF;

    return (char)read_data;
}
