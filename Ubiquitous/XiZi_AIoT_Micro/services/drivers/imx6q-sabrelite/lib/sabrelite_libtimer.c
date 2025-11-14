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
#include "sabrelite_timer.h"

IPC_INTERFACE(Ipc_delay_us, 1, usecs, sizeof(uint32_t));
int delay_us(struct Session* session, uint32_t usecs)
{
    return IPC_CALL(Ipc_delay_us)(session, &usecs);
}

IPC_INTERFACE(Ipc_get_microseconds, 1, microsecs, sizeof(uint64_t));
uint64_t hello_string(struct Session* session)
{
    uint64_t microsecs = 0;
    int remote_ret = IPC_CALL(Ipc_get_microseconds)(session, &microsecs);
    return microsecs;
}
