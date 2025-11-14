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
 * @file share_page.h
 * @brief support share page for tasks
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: share_page.h
Description: support share page for tasks
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "actracer.h"
#include "ksemaphore.h"
#include "list.h"
#include "task.h"

/// @brief userland session info copy
struct Session {
    uintptr_t id;
    int capacity;
    int head;
    int tail;
    void* buf;
};

#define SERVER_SESSION_BACKEND(session) CONTAINER_OF(session, struct session_backend, server_side)
#define CLIENT_SESSION_BACKEND(session) CONTAINER_OF(session, struct session_backend, client_side)

struct server_session {
    struct double_list_node node; // list node of server task's ipc pipes
    uintptr_t buf_addr;
    int capacity;
    int head;
    int tail;
    bool closed;
};

struct client_session {
    struct double_list_node node; // list node of client task's ipc pipes
    uintptr_t buf_addr;
    int capacity;
    bool closed;
};

/// @brief kernel data struct to represent a ipc pipe, listed in client's task
struct session_backend {
    struct server_session server_side;
    struct client_session client_side;
    int session_id; // id of this session
    int nr_pages; // pages used by this pipe
    struct Thread* client; // client of this pipe
    struct Thread* server; // server of this pipe

    sem_id_t client_sem_to_wait;
    uintptr_t buf_kernel_addr;
};

struct SharePageRightGroup {
    struct TraceTag dcache_driver_tag;
    struct TraceTag mmu_driver_tag;
};

struct XiziSharePageManager {
    struct session_backend* (*create_share_pages)(struct Thread* client, struct Thread* server, const int capacity);
    void (*unmap_task_share_pages)(struct Thread* task, const uintptr_t task_vaddr, const int nr_pages);
    int (*delete_share_pages)(struct session_backend* session_backend);

    uintptr_t (*task_map_pages)(struct Thread* task, const uintptr_t vaddr, const uintptr_t paddr, const int nr_pages, const int is_dev);
};
extern struct XiziSharePageManager xizi_share_page_manager;

int module_share_page_init(struct SharePageRightGroup* right_group);

static inline void client_close_session(struct Thread* thd, struct client_session* cli_sess)
{
    assert(cli_sess != NULL);
    struct session_backend* sess_backend = CLIENT_SESSION_BACKEND(cli_sess);
    assert(sess_backend->client == thd);
    assert(cli_sess->closed == false);
    cli_sess->closed = true;
    xizi_share_page_manager.delete_share_pages(sess_backend);
}

static inline void server_close_session(struct Thread* thd, struct server_session* svr_sess)
{
    assert(svr_sess != NULL);
    struct session_backend* sess_backend = SERVER_SESSION_BACKEND(svr_sess);
    assert(sess_backend->server == thd);
    assert(svr_sess->closed == false);
    svr_sess->closed = true;
    xizi_share_page_manager.delete_share_pages(sess_backend);
}