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
#include <string.h>

#include "libipc.h"
#include "libserial.h"
#include "usyscall.h"

IPC_SERVICES(IpcTestNull, Ipc_test_null);
#define TEST_BUF_SIZE 32

static char test_null_server_name[] = "TestNullServer";
static char test_null_client_name[] = "TestNullClient";

// client side
static char buf[TEST_BUF_SIZE];

IPC_INTERFACE(Ipc_test_null, 1, ptr, sizeof(buf));
int test_null(struct Session* session, char* ptr)
{
    return IPC_CALL(Ipc_test_null)(session, ptr);
}

// client thread
int server_thread(int argc, char** argv);
int main(int argc, char** argv)
{
    struct Session session;
    bool server_enabled = false;
    while (connect_session(&session, test_null_server_name, 4096) < 0) {
        if (!server_enabled) {
            char* server_param[] = { test_null_server_name, NULL };
            if (thread(server_thread, test_null_server_name, server_param) >= 0) {
                server_enabled = true;
            }
        }
    }

    printf("[%s] Call using NULL ptr.\n", test_null_client_name);
    test_null(&session, NULL);
    printf("[%s] Call using non-NULL ptr.\n", test_null_client_name);
    test_null(&session, buf);

    exit(0);
    return 0;
}

// server side
int IPC_DO_SERVE_FUNC(Ipc_test_null)(void* ptr)
{
    if (ptr == NULL) {
        printf("[%s]: A NULL ptr ipc call.\n", test_null_server_name);
    } else {
        printf("[%s]: A non-NULL ptr ipc call.\n", test_null_server_name);
    }

    return 0;
}

IPC_SERVER_INTERFACE(Ipc_test_null, 1);
IPC_SERVER_REGISTER_INTERFACES(IpcTestNull, 1, Ipc_test_null);

// server threads
int server_thread(int argc, char** argv)
{
    if (register_server(test_null_server_name) < 0) {
        printf("[%s] Register %s server failed.\n", test_null_server_name, test_null_server_name);
        exit(1);
        return 1;
    }
    ipc_server_loop(&IpcTestNull);

    exit(0);
    return 0;
}
