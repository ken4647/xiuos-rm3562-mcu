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
 * @file sys_register_as_server.c
 * @brief server task registration
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_register_as_server.c
Description: server task registration
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <string.h>

#include "actracer.h"
#include "assert.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

int sys_register_as_server(char* name)
{
    // get server thread
    struct Thread* server = cur_cpu()->task;
    assert(server != NULL);

    // get server tag owner
    struct TraceTag server_identifier_set_tag;
    if (!AchieveResourceTag(&server_identifier_set_tag, RequireRootTag(), "softkernel/server-identifier")) {
        panic("Server identifier not initialized.\b");
    }
    assert(server_identifier_set_tag.meta != NULL);

    // create server tag under server tag owner
    if (!CreateResourceTag(&server->server_identifier, &server_identifier_set_tag, name, TRACER_SERVER_IDENTITY_AC_RESOURCE, server)) {
        return -1;
    }

    return 0;
}
