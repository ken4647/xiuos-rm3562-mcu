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
 * @file sys_close_session.c
 * @brief close session syscall
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: sys_close_session.c
Description: close session syscall
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "assert.h"
#include "multicores.h"
#include "share_page.h"
#include "syscall.h"
#include "task.h"

/// @brief close a session syscall
/// @warning best to be called by a client
/// @param cur_task
/// @param session
/// @return
int sys_close_session(struct Thread* cur_task, struct Session* session)
{
    assert(cur_task != NULL);
    /* check if session is available */
    if (session->buf == NULL || (uintptr_t)session->buf < USER_IPC_SPACE_BASE || (uintptr_t)session->buf > USER_IPC_SPACE_TOP) {
        return -1;
    }

    struct session_backend* session_backend = NULL;

    /* check if session is a client one or a server one */
    RbtNode* client_session_node = rbt_search(&cur_task->cli_sess_map, session->id);
    if (client_session_node != NULL) {
        struct client_session* client_session = (struct client_session*)client_session_node->data;
        if (CLIENT_SESSION_BACKEND(client_session)->session_id != session->id || //
            client_session->buf_addr != (uintptr_t)session->buf) {
            ERROR("Error closing session from %s: Invalid session\n", cur_task->name);
            return -1;
        }

        /* close client session */
        session_backend = CLIENT_SESSION_BACKEND(client_session);
        assert(session_backend->client == cur_task);
        assert(client_session->closed == false);
        client_session->closed = true;
        xizi_share_page_manager.delete_share_pages(session_backend);

        struct Thread* server_to_info = session_backend->server;
        if (!enqueue(&server_to_info->sessions_to_be_handle, 0, (void*)&session_backend->server_side)) {
            // @todo fix memory leak
        } else {
            assert(!queue_is_empty(&server_to_info->sessions_to_be_handle));
            THREAD_TRANS_STATE(server_to_info, TRANS_WAKING);
        }
    }

    RbtNode* server_session_node = rbt_search(&cur_task->svr_sess_map, session->id);
    if (server_session_node != NULL) {
        struct server_session* server_session = (struct server_session*)server_session_node->data;
        if (SERVER_SESSION_BACKEND(server_session)->session_id != session->id || //
            server_session->buf_addr != (uintptr_t)session->buf) {
            ERROR("Error closing session from %s: Invalid session\n", cur_task->name);
            return -1;
        }

        session_backend = SERVER_SESSION_BACKEND(server_session);
        assert(session_backend->server == cur_task);
        assert(server_session->closed == false);
        server_session->closed = true;
        xizi_share_page_manager.delete_share_pages(session_backend);
    }

    /* close this session */
    if (session_backend == NULL) {
        return -1;
    }

    return 0;
}