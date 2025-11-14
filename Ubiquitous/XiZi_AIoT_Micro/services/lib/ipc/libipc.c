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
 * @file libipc.c
 * @brief package seesion for inter process call perpose
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: libipc.c
Description: package seesion for inter process call perpose
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <string.h>

#include "libipc.h"
#include "libserial.h"
#include "usyscall.h"

/// @warning this file doesnt support multi threads

/// ipc part

struct IpcMsg* new_ipc_msg(struct Session* session, const int argc, const int* arg_size)
{
    int len = IPC_ARG_INFO_BASE_OFFSET;

    int arg_info_offset = len; // start of arg info
    len += argc * sizeof(struct IpcArgInfo);

    int arg_buf_offset = len; // start of arg buffer
    for (int i = 0; i < argc; i++) {
        len += arg_size[i];
    }
    struct IpcMsg* buf = session_alloc_buf(session, len);
    if (buf == NULL) {
        return NULL;
    }
    buf->header.len = len;
    buf->header.done = 0;
    buf->header.valid = 0;

    for (int i = 0; i < argc; i++) {
        struct IpcArgInfo* arg_info = (struct IpcArgInfo*)((char*)buf + arg_info_offset);
        arg_info->len = arg_size[i];
        arg_info->offset = arg_buf_offset;

        // update arg_info_offset and arg_buf_offset
        arg_info_offset += sizeof(struct IpcArgInfo);
        arg_buf_offset += arg_size[i];
    }
    buf->header.nr_args = argc;
    buf->header.init = 1;
    return buf;
}

/// @brief set nth arg in ipc_msg with data
/// @param msg
/// @param arg_num start with 0 for first arg
/// @param data
/// @param len data buffer len
/// @return
bool ipc_msg_set_nth_arg(struct IpcMsg* msg, const int arg_num, const void* const data, const int len)
{
    if (arg_num >= msg->header.nr_args) {
        printf("[%s] IPC: arg_num out of msg range, arg_num: %d, nr_args: %u\n", __func__, arg_num, msg->header.nr_args);
        return false;
    }
    struct IpcArgInfo* nth_arg_info = IPCMSG_ARG_INFO(msg, arg_num);
    if (len < 0 || (uint32_t)len > (uint32_t)nth_arg_info->len) {
        printf("[%s] IPC: size of arg out of buffer range, given len: %d, len %u\n", __func__, len, nth_arg_info->len);
        return false;
    }

    void* buf = ipc_msg_get_nth_arg_buf(msg, arg_num);

    // handle attributes of different params
    if (data == NULL) {
        nth_arg_info->null_ptr = 1;
        memset(buf, 0x0, len);
        return true;
    } else {
        nth_arg_info->null_ptr = 0;
    }

    memmove(buf, data, len);
    return true;
}

/// @brief get nth arg in ipc_msg and move value to data
/// @param msg
/// @param arg_num start with 0 for first arg
/// @param data
/// @param len data buffer len
/// @return
bool ipc_msg_get_nth_arg(struct IpcMsg* msg, const int arg_num, void* data, const int len)
{
    if (arg_num >= msg->header.nr_args) {
        printf("[%s] IPC: arg_num out of msg range", __func__);
        return false;
    }
    struct IpcArgInfo* nth_arg_info = IPCMSG_ARG_INFO(msg, arg_num);
    if (len > nth_arg_info->len) {
        printf("[%s] IPC: size of arg out of buffer range", __func__);
        return false;
    }

    // handle null ptr: do nothing
    if (nth_arg_info->null_ptr == 1) {
        return true;
    }

    void* buf = ipc_msg_get_nth_arg_buf(msg, arg_num);
    memmove(data, buf, len);
    return true;
}

void ipc_msg_send_wait(struct Session* session, struct IpcMsg* msg)
{
    msg->header.magic = IPC_MSG_MAGIC;
    msg->header.valid = 1;
    msg->header.done = 0;
    while (msg->header.done == 0) {
        /// @todo syscall yield with prio decrease
        wait_session_call(session);
    }
    assert(msg->header.done == 1);
}

void ipc_msg_send_nowait(struct IpcMsg* msg)
{
    msg->header.magic = IPC_MSG_MAGIC;
    msg->header.valid = 1;
    msg->header.done = 0;
}

int ipc_session_wait(struct Session* session)
{
    struct IpcMsg* msg = IPCSESSION_MSG(session);
    while (msg->header.done == 0) {
        /// @todo syscall yield with prio decrease
        wait_session_call(session);
    }
    assert(msg->header.done == 1);
    return msg->header.ret_val;
}

static int cur_sess_id = -1;
static struct IpcMsg* ipc_server_loop_cur_msg = NULL;
int cur_session_id(void)
{
    return cur_sess_id;
}

void ipc_server_loop(struct IpcNode* ipc_node)
{
    struct Session session_list[NR_MAX_SESSION];
    memset(session_list, 0, sizeof(session_list));
    for (;;) {
        /* if connect sessions are greater than NR_MAX_SESSION,
            a full round will require multiple polls.
            Eg.(3 polls to with 6 sessions and NR_MAX_SESSION = 4):
                [0, 1, 2, 3], [4, 5, 0, 1], [2, 3, 4, 5]
        */
        poll_session(session_list, NR_MAX_SESSION);
        /* handle each session */
        for (int i = 0; i < NR_MAX_SESSION; i++) {
            if (session_list[i].buf == NULL) {
                break;
            }
            cur_sess_id = session_list[i].id;
            ipc_server_loop_cur_msg = IPCSESSION_MSG(&session_list[i]);
            if (ipc_server_loop_cur_msg->header.magic == IPC_MSG_MAGIC && ipc_server_loop_cur_msg->header.valid == 1 && //
                ipc_server_loop_cur_msg->header.done == 1) {
                session_forward_head(&session_list[i], ipc_server_loop_cur_msg->header.len);
                ipc_server_loop_cur_msg = IPCSESSION_MSG(&session_list[i]);
            }
            /* handle every message in current session
                a session could be delay in case one of its message(current message) needs to wait for an interrupt message's arrival
                interfaces[opcode] should explicitly call delay_session() and return to delay this session
            */
            while (ipc_server_loop_cur_msg->header.magic == IPC_MSG_MAGIC && ipc_server_loop_cur_msg->header.valid == 1 && //
                ipc_server_loop_cur_msg->header.handling == 0 && ipc_server_loop_cur_msg->header.done == 0) {
                if (session_used_size(&session_list[i]) == 0 && session_forward_tail(&session_list[i], ipc_server_loop_cur_msg->header.len) < 0) {
                    break;
                }

                // this is a message needs to handle
                if (ipc_node->interfaces[ipc_server_loop_cur_msg->header.opcode]) {
                    ipc_node->interfaces[ipc_server_loop_cur_msg->header.opcode](ipc_server_loop_cur_msg);
                } else {
                    printf("Unsupport opcode(%u) for server: %s\n", ipc_server_loop_cur_msg->header.opcode, ipc_node->name);
                }
                // current msg is a message that needs to ignore
                // finish this message in server's perspective
                if (ipc_server_loop_cur_msg->header.done == 0) {
                    break;
                }
                session_forward_head(&session_list[i], ipc_server_loop_cur_msg->header.len);
                ipc_server_loop_cur_msg = IPCSESSION_MSG(&session_list[i]);
            }
            // stop handle this session
            cur_sess_id = -1;
            ipc_server_loop_cur_msg = NULL;
        }
    }
}

// utils
void _ipc_addr_to_buf(uintptr_t addr, char buf[17])
{
    int buf_idx = 0;
    while (addr != 0) {
        int x = addr % 16;
        if (x < 10) {
            buf[buf_idx] = x + '0';
        } else {
            buf[buf_idx] = x - 10 + 'A';
        }
        buf_idx++;
        addr /= 16;
    }
    buf[buf_idx] = '\0';
}

uintptr_t _ipc_buf_to_addr(char* buf)
{
    uintptr_t addr = 0;
    int buf_idx = 0;
    uintptr_t multiplier = 1;
    while (buf_idx < 17 && buf[buf_idx] != '\0') {
        uint8_t x = (uint8_t)buf[buf_idx];
        if (x >= '0' && x <= '9') {
            x -= '0';
        } else if (x >= 'A' && x <= 'F') {
            x -= 'A';
            x += 10;
        }
        addr += (uintptr_t)x * multiplier;
        multiplier *= 16;
        buf_idx++;
    }
    return addr;
}