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

#include "libipc.h"
#include "lwip/sockets.h"

IPC_SERVICES(IpcLWIPServer, Ipc_socket, Ipc_bind,Ipc_connect,Ipc_listen,Ipc_accept,
            Ipc_read, Ipc_recv, Ipc_recfrom, Ipc_sendto, Ipc_send, Ipc_write, Ipc_close, Ipc_setsockopt)

int ipc_socket(struct Session* session,
             int domain, int type, int protocol);

int ipc_bind(struct Session* session, int s, struct sockaddr *name, socklen_t namelen);

int ipc_connect(struct Session* session, int s, struct sockaddr *name, socklen_t namelen);

int ipc_listen(struct Session* session, int s, int backlog);

int ipc_accept(struct Session* session, int s, struct sockaddr *addr, socklen_t *addrlen);

ssize_t ipc_read(struct Session* session, int s, void *mem, size_t len);

ssize_t ipc_recv(struct Session* session, int s, void *mem, size_t len, int flags);

ssize_t ipc_recvfrom(struct Session* session, int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);

ssize_t ipc_sendto(struct Session* session, int s, void *data, size_t size, int flags, struct sockaddr *to, socklen_t tolen);

ssize_t ipc_send(struct Session* session, int s, void *data, size_t size, int flags);

ssize_t ipc_write(struct Session* session, int s, void *data, size_t size);

int ipc_close(struct Session* session, int s);

int ipc_setsockopt(struct Session* session, int s, int level, int optname, void *optval, socklen_t optlen);
