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
#include "pingpong_service.h"

IPC_INTERFACE(Ipc_pingpong, 1, str, strlen(str));
int pingpong(struct Session* session, char* str)
{
    return IPC_CALL(Ipc_pingpong)(session, str);
}

void pingpong_nowait(struct Session* session, char* str)
{
    struct IpcMsg* msg = IPC_CREATE_MSG_FUNC(Ipc_pingpong)(session, str);
    ipc_msg_set_nth_arg(msg, 0, str, strlen(str));
    ipc_msg_set_opcode(msg, Ipc_pingpong);
    ipc_msg_send_nowait(msg);
}
