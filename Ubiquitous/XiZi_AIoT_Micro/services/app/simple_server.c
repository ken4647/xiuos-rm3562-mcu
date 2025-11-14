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
#include "simple_service.h"
#include "usyscall.h"

/// @warning all the parameters should in the form of pointers
///          for the true storing memory of parameters is session(shared memory between tasks)
int IPC_DO_SERVE_FUNC(Ipc_add)(int* a, int* b)
{
    return *a + *b;
}

int IPC_DO_SERVE_FUNC(Ipc_hello_string)(char* buf, int* len)
{
    static char hello_string[] = "Hello, This is server!";
    int copy_len = sizeof(hello_string) > *len ? *len : sizeof(hello_string);
    memcpy(buf, hello_string, copy_len);
    return 0;
}

IPC_SERVER_INTERFACE(Ipc_add, 2);
IPC_SERVER_INTERFACE(Ipc_hello_string, 2);
IPC_SERVER_REGISTER_INTERFACES(IpcSimpleServer, 2, Ipc_hello_string, Ipc_add);

int main(int argc, char* argv[])
{
    if (register_server("SimpleServer") < 0) {
        printf("register server name: %s failed.\n", "SimpleServer");
        exit(1);
    }
    ipc_server_loop(&IpcSimpleServer);

    // never reached
    exit(0);
    return 0;
}
