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
 * @file sys_poll_session.c
 * @brief server poll its connected sessions
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_poll_session.c
Description: server poll its connected sessions
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "assert.h"
#include "ipc.h"
#include "multicores.h"
#include "share_page.h"
#include "syscall.h"
#include "task.h"

extern bool ksemaphore_wait(struct XiziSemaphorePool* sem_pool, struct Thread* thd, sem_id_t sem_id);
int sys_poll_session(struct Session* userland_session_arr, int arr_capacity)
{
    struct Thread* cur_task = cur_cpu()->task;
    if (cur_task == NULL) {
        ERROR("%s by killed task\n");
        return -1;
    }

    /* update old sessions */
    int cur_userland_idx = 0;
    while (!queue_is_empty(&cur_task->sessions_in_handle)) {
        struct server_session* server_session = (struct server_session*)queue_front(&cur_task->sessions_in_handle)->data;
        assert(server_session != NULL);

        // wrong session info
        if (userland_session_arr[cur_userland_idx].id != SERVER_SESSION_BACKEND(server_session)->session_id || //
            (uintptr_t)userland_session_arr[cur_userland_idx].buf != server_session->buf_addr) {
            ERROR("mismatched old session from %s, user buf: %x, server buf: %x\n", cur_task->name, userland_session_arr[cur_userland_idx].buf, server_session->buf_addr);
        } else {
            // update session_backend
            ksemaphore_signal(&xizi_task_manager.semaphore_pool, SERVER_SESSION_BACKEND(server_session)->client_sem_to_wait);

            server_session->head = userland_session_arr[cur_userland_idx].head;
            server_session->tail = userland_session_arr[cur_userland_idx].tail;
            userland_session_arr[cur_userland_idx].buf = NULL;
            userland_session_arr[cur_userland_idx].id = -1;
        }

        assert(dequeue(&cur_task->sessions_in_handle));
        cur_userland_idx++;
    }

    /* poll with new sessions */
    cur_userland_idx = 0;
    while (!queue_is_empty(&cur_task->sessions_to_be_handle)) {
        if (cur_userland_idx == arr_capacity) {
            break;
        }

        struct server_session* server_session = (struct server_session*)queue_front(&cur_task->sessions_to_be_handle)->data;
        assert(server_session != NULL);

        if (SERVER_SESSION_BACKEND(server_session)->client_side.closed) {
            // client had closed it, then server will close it too
            struct session_backend* session_backend = SERVER_SESSION_BACKEND(server_session);
            assert(session_backend->server == cur_task);
            assert(session_backend->client == NULL);
            assert(server_session->closed == false);
            server_session->closed = true;
            xizi_share_page_manager.delete_share_pages(session_backend);
            dequeue(&cur_task->sessions_to_be_handle);
            continue;
        }

        userland_session_arr[cur_userland_idx] = (struct Session) {
            .buf = (void*)server_session->buf_addr,
            .capacity = server_session->capacity,
            .head = server_session->head,
            .tail = server_session->tail,
            .id = SERVER_SESSION_BACKEND(server_session)->session_id,
        };

        if (!enqueue(&cur_task->sessions_in_handle, 0, (void*)server_session)) {
            userland_session_arr[cur_userland_idx].buf = NULL;
            userland_session_arr[cur_userland_idx].id = 0;
            break;
        }
        assert(dequeue(&cur_task->sessions_to_be_handle));
        cur_userland_idx++;
    }

    // end of userland copy
    if (cur_userland_idx < arr_capacity) {
        userland_session_arr[cur_userland_idx].buf = NULL;
    }

    if (queue_is_empty(&cur_task->sessions_in_handle) && queue_is_empty(&cur_task->sessions_to_be_handle)) {
        THREAD_TRANS_STATE(cur_task, BLOCKED);
    }
    return 0;
}