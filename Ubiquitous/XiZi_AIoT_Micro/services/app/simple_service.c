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
#include "simple_service.h"

IPC_INTERFACE(Ipc_add, 2, a, b, sizeof(int), sizeof(int));
int add(struct Session* session, int a, int b)
{
    return IPC_CALL(Ipc_add)(session, &a, &b);
}

IPC_INTERFACE(Ipc_hello_string, 2, buf, len, *(int*)len, sizeof(int));
int hello_string(struct Session* session, char* buf, int len)
{
    return IPC_CALL(Ipc_hello_string)(session, buf, &len);
}

struct IpcMsg* hello_string_nowait(struct Session* session, char** buf, int len)
{
    struct IpcMsg* msg = IPC_CREATE_MSG_FUNC(Ipc_hello_string)(session, *buf, &len);
    *buf = ipc_msg_get_nth_arg_buf(msg, 0);
    ipc_msg_set_nth_arg(msg, 1, &len, sizeof(int));
    ipc_msg_set_opcode(msg, Ipc_hello_string);
    ipc_msg_send_nowait(msg);
    return msg;
}
