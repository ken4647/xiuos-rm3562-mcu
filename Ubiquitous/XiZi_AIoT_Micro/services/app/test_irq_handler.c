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

#include "test_irq.h"

static bool has_one_interrupt = false;
int IPC_DO_SERVE_FUNC(Ipc_intr_3)(void* ignore)
{
    printf("TEST_SW_HDLR: In %s()\n", __func__);
    has_one_interrupt = true;
    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_wait_intr_3)(void* ignore)
{
    // delay the this handle
    while (!has_one_interrupt) {
        yield(SYS_TASK_YIELD_NO_REASON);
    }

    // serve can be done by now
    has_one_interrupt = false;

    return 0;
}

IPC_SERVER_INTERFACE(Ipc_intr_3, 1);
IPC_SERVER_THREAD_INTERFACE(Ipc_wait_intr_3, 1);
IPC_SERVER_REGISTER_INTERFACES(IpcSwIntrHandler, 2, Ipc_intr_3, Ipc_wait_intr_3);
int main()
{
    if (register_irq(SW_INTERRUPT_3, Ipc_intr_3) < 0) {
        printf("TEST_SW_HDLR: bind failed");
        exit(1);
    }

    static char prog_name[] = "TestIRQ";
    if (register_server("TestIRQ") < 0) {
        printf("register server name: %s failed.\n", prog_name);
        exit(1);
    }

    ipc_server_loop(&IpcSwIntrHandler);

    exit(0);
    return 0;
}