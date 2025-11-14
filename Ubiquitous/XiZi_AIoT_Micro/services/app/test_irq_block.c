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

IPC_INTERFACE(Ipc_wait_intr_3, 1, ignore, 0);
int wait_intr(struct Session* session)
{
    return IPC_CALL(Ipc_wait_intr_3)(session, NULL);
}

static char prog_name[] = "TEST_IRQ_BLOCK";
int main(int argc, char* argv[])
{
    struct Session session;
    if (connect_session(&session, "TestIRQ", 4096) < 0) {
        printf("connect session failed\n");
        exit(1);
    }

    printf("%s start waiting for IRQ.\n", prog_name);
    wait_intr(&session);
    printf("%s return from waiting for IRQ.\n", prog_name);

    exit(0);
}