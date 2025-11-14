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
#include <stdbool.h>

#include "libserial.h"
#include "pingpong_service.h"
#include "usyscall.h"

/// @warning all the parameters should in the form of pointers
///          for the true storing memory of parameters is session(shared memory between tasks)
int IPC_DO_SERVE_FUNC(Ipc_pingpong)(char* str)
{
    return 0;
}

IPC_SERVER_INTERFACE(Ipc_pingpong, 1);
IPC_SERVER_REGISTER_INTERFACES(IpcPingPongServer, 1, Ipc_pingpong);

int main(int argc, char* argv[])
{
    if (register_server("PingPongServer") < 0) {
        printf("register server name: %s failed.\n", "PingPong");
        exit(1);
    }
    ipc_server_loop(&IpcPingPongServer);

    // never reached
    exit(0);
    return 0;
}