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
 * @file sys_wait_session.c
 * @brief
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
#include "multicores.h"
#include "share_page.h"

#include "syscall.h"

extern bool ksemaphore_wait(struct XiziSemaphorePool* sem_pool, struct Thread* thd, sem_id_t sem_id);
int sys_wait_session(struct Session* userland_session)
{
    struct Thread* cur_task = cur_cpu()->task;

    RbtNode* client_session_node = rbt_search(&cur_task->cli_sess_map, userland_session->id);
    if (client_session_node == NULL) {
        ERROR("Error waiting session from %s: Invalid session %d\n", cur_task->name, userland_session->id);
        return -1;
    }

    struct client_session* client_session = (struct client_session*)client_session_node->data;
    if (CLIENT_SESSION_BACKEND(client_session)->session_id != userland_session->id || //
        client_session->buf_addr != (uintptr_t)userland_session->buf) {
        ERROR("Error waiting session from %s: Invalid session %d\n", cur_task->name, userland_session->id);
        return -1;
    }

    /* handle calling */
    struct session_backend* session_backend = CLIENT_SESSION_BACKEND(client_session);
    struct Thread* server_to_call = session_backend->server;
    if (!enqueue(&server_to_call->sessions_to_be_handle, 0, (void*)&session_backend->server_side)) {
        sys_exit(cur_task);
        return -1;
    }
    assert(!queue_is_empty(&server_to_call->sessions_to_be_handle));

    ksemaphore_wait(&xizi_task_manager.semaphore_pool, cur_task, session_backend->client_sem_to_wait);
    THREAD_TRANS_STATE(server_to_call, TRANS_WAKING);

    return 0;
}