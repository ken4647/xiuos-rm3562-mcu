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
 * @file session.c
 * @brief session
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: session.c
Description: session
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <assert.h>

#include "session.h"
#include "usyscall.h"

int connect_session(struct Session* _session, char* _path, int _capacity)
{
    return session(_path, _capacity, _session);
}

int free_session(struct Session* session)
{
    return close_session(session);
}

// ipc_msg alloc_buffer(ipc_session)

void* session_alloc_buf(struct Session* session, int len)
{
    if (len < 0 || len > session_remain_capacity(session)) {
        return NULL;
    }
    void* buf = (void*)((uintptr_t)session->buf + session->tail);
    // we mapped double size of page, so it's ok to write buffer directly
    memset(buf, 0, len);
    session_forward_tail(session, len);
    return buf;
}

bool session_free_buf(struct Session* session, int len)
{
    if (len < 0 || len > session_used_size(session)) {
        return false;
    }
    assert(session_forward_head(session, len) != -1);
    return true;
}
