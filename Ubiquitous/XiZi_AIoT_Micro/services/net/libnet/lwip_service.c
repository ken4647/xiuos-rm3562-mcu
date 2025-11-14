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
#include "lwip_service.h"
#include <string.h>

IPC_INTERFACE(Ipc_socket, 3, domain, type, protocol, sizeof(int), sizeof(int), sizeof(int));
int ipc_socket(struct Session* session, int domain, int type, int protocol){
    return IPC_CALL(Ipc_socket)(session, &domain, &type, &protocol);
}

IPC_INTERFACE(Ipc_bind, 3, s, name, namelen, sizeof(int), sizeof(struct sockaddr), sizeof(socklen_t));
int ipc_bind(struct Session* session, int s, struct sockaddr *name, socklen_t namelen){
    return IPC_CALL(Ipc_bind)(session, &s, name, &namelen);
}

IPC_INTERFACE(Ipc_connect, 3, s, name, namelen, sizeof(int), sizeof(struct sockaddr), sizeof(socklen_t));
int ipc_connect(struct Session* session, int s, struct sockaddr *name, socklen_t namelen){
    return IPC_CALL(Ipc_connect)(session, &s, name, &namelen);
}

IPC_INTERFACE(Ipc_listen, 2, s, backlog, sizeof(int), sizeof(int));
int ipc_listen(struct Session* session, int s, int backlog){
    return IPC_CALL(Ipc_listen)(session, &s, &backlog);
}

IPC_INTERFACE(Ipc_accept, 3, s, addr, addlen, sizeof(int), sizeof(struct sockaddr), sizeof(socklen_t));
int ipc_accept(struct Session* session, int s, struct sockaddr *addr, socklen_t *addrlen){
    return IPC_CALL(Ipc_accept)(session, &s, addr, addrlen);
}

IPC_INTERFACE(Ipc_read, 3, s, mem, len, sizeof(int), *(size_t *)len, sizeof(size_t));
ssize_t ipc_read(struct Session* session, int s, void *mem, size_t len) {
    return IPC_CALL(Ipc_read)(session, &s, mem, &len);
}

IPC_INTERFACE(Ipc_recv, 4, s, mem, len, flags, sizeof(int), *(size_t *)len, sizeof(size_t), sizeof(int));
ssize_t ipc_recv(struct Session* session, int s, void *mem, size_t len, int flags){
    return IPC_CALL(Ipc_recv)(session, &s, mem, &len, &flags);
}

IPC_INTERFACE(Ipc_recfrom, 6, s, mem, len, flags, from, fromlen, sizeof(int), *(size_t *)len, sizeof(size_t), sizeof(int), sizeof(struct sockaddr), sizeof(socklen_t));
ssize_t ipc_recvfrom(struct Session* session, int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen){
    return IPC_CALL(Ipc_recfrom)(session, &s, mem, &len, &flags, from, fromlen);
}

IPC_INTERFACE(Ipc_sendto, 6, s, data, size, flags, to, tolen, sizeof(int), *(size_t *)size, sizeof(size_t), sizeof(int), sizeof(struct sockaddr), sizeof(socklen_t));
ssize_t ipc_sendto(struct Session* session, int s, void *data, size_t size, int flags, struct sockaddr *to, socklen_t tolen){
    return IPC_CALL(Ipc_sendto)(session, &s, data, &size, &flags, to, &tolen);
}

IPC_INTERFACE(Ipc_send, 4, s, data, size, flags, sizeof(int), *(size_t *)size, sizeof(size_t), sizeof(int));
ssize_t ipc_send(struct Session* session, int s, void *data, size_t size, int flags){
    return IPC_CALL(Ipc_send)(session, &s, data, &size, &flags);
}

IPC_INTERFACE(Ipc_write, 3, s, data, size, sizeof(int), *(size_t *)size, sizeof(size_t));
ssize_t ipc_write(struct Session* session, int s, void *data, size_t size){
    return IPC_CALL(Ipc_write)(session, &s, data, &size);
}

IPC_INTERFACE(Ipc_close, 1, s, sizeof(int));
int ipc_close(struct Session* session, int s){
    return IPC_CALL(Ipc_close)(session, &s);
}

IPC_INTERFACE(Ipc_setsockopt, 5, s, level, optname, optval, optlen, sizeof(int), sizeof(int), sizeof(int), *(socklen_t*)optlen, sizeof(socklen_t));
int ipc_setsockopt(struct Session* session, int s, int level, int optname, void *optval, socklen_t optlen){
    return IPC_CALL(Ipc_setsockopt)(session, &s, &level, &optname, optval, &optlen);
}