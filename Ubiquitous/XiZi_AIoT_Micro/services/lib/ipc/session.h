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
 * @file session.h
 * @brief session header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: session.h
Description: session header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libserial.h"

struct Session {
    uintptr_t id;
    int capacity;
    int head;
    int tail;
    void* buf;
};

__attribute__((__always_inline__)) static inline int session_used_size(struct Session* session)
{
    return ((session->tail + session->capacity) - session->head) % session->capacity;
}

__attribute__((__always_inline__)) static inline int session_remain_capacity(struct Session* session)
{
    return session->capacity - session_used_size(session);
}

__attribute__((__always_inline__)) static inline int session_forward_head(struct Session* session, int len)
{
    if (len > session_used_size(session)) {
        printf("forward head with too much size, session used size: %d\n", session_used_size(session));
        return -1;
    }
    session->head = (session->head + len) % session->capacity;
    return session->head;
}

__attribute__((__always_inline__)) static inline int session_forward_tail(struct Session* session, int len)
{
    if (len > session_remain_capacity(session)) {
        printf("forward tail with too much size\n");
        return -1;
    }
    session->tail = (session->tail + len) % session->capacity;
    return session->tail;
}

int connect_session(struct Session* session, char* path, int capacity);
int free_session(struct Session* session);
void* session_alloc_buf(struct Session* session, int len);
bool session_free_buf(struct Session* session, int len);