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
 * @file ipc.h
 * @brief use share memory page to support inter process communication
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: ipc.h
Description: use share memory page to support inter process communication
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include <stdint.h>

/* the content of this file must be the same with libipc.h */

#define IPC_MSG_MAGIC 0xABCDDCBA

typedef struct {
    union {
        uint64_t header;
        struct {
            uint64_t valid : 1; // for server to peek new msg
            uint64_t done : 1; // for client to check request done
            uint64_t init : 1; // for client to check request done
            uint64_t handling : 1;
            uint64_t nr_args : 4;
            uint64_t opcode : 8;
            uint64_t len : 16;
            uint64_t magic : 32;
        };
    };
    int32_t ret_val;
} __attribute__((packed)) ipc_msg_header;

struct IpcArgInfo {
    uint16_t offset;
    uint16_t len;
    union {
        uint16_t attr;
        struct {
            uint16_t null_ptr : 1;
            uint16_t reserved : 15;
        };
    };
} __attribute__((packed));

/* [header, ipc_arg_buffer_len[], ipc_arg_buffer[]] */
struct IpcMsg {
    ipc_msg_header header;
    uintptr_t buf[];
} __attribute__((packed));
enum {
    IPC_ARG_INFO_BASE_OFFSET = sizeof(ipc_msg_header),
};