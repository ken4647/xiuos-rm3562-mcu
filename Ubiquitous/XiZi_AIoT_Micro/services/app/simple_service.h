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

IPC_SERVICES(IpcSimpleServer, Ipc_add, Ipc_hello_string);

int add(struct Session* session, int a, int b);
int hello_string(struct Session* session, char* buf, int len);
struct IpcMsg* hello_string_nowait(struct Session* session, char** buf, int len);
