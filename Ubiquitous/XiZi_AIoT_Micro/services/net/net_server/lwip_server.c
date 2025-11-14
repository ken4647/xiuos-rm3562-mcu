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
#include "libserial.h"
#include "usyscall.h"
#include "lwip_service.h"

#include "lwip/init.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "netif/ethernet.h"
#include "sys_arch.h"
#include "lwip/sockets.h"

extern struct netif gnetif;

int IPC_DO_SERVE_FUNC(Ipc_socket)(int *domain, int *type, int *protocol){
    return socket(*domain, *type, *protocol);
}

int IPC_DO_SERVE_FUNC(Ipc_bind)(int *s, const struct sockaddr *name, socklen_t *namelen){
    return bind(*s, name, *namelen);
}

int IPC_DO_SERVE_FUNC(Ipc_connect)(int* s, const struct sockaddr* name, socklen_t* namelen){
    return connect(*s, name, *namelen);
}

int IPC_DO_SERVE_FUNC(Ipc_listen)(int* s, int* backlog){
    return listen(*s, *backlog);
}
// int serve_accept(struct Session* session, int *s, struct sockaddr *addr, socklen_t *addrlen) {
//     session_finish_handle(session, accept(*s, addr, addrlen));
//     exit(0);
//     return 0;
// }

// static char accept_thread[] = "accept_thread";
int IPC_DO_SERVE_FUNC(Ipc_accept)(int* s, struct sockaddr* addr, socklen_t* addrlen){
    // if(!is_cur_handler_been_delayed()) {
    //     char* param[] = {s, addr, addrlen, NULL};
    //     if(thread(serve_accept, accept_thread, param) < 0) {
    //         return -1;
    //     }
    // }
    // delay_session();
    return accept(*s, addr, addrlen);
}

int IPC_DO_SERVE_FUNC(Ipc_read)(int* s, void* mem, size_t* len){
    return read(*s, mem, *len);
}

int IPC_DO_SERVE_FUNC(Ipc_recv)(int* s, void* mem, size_t* len, int* flags){
    return recv(*s, mem, *len, *flags);
}

int IPC_DO_SERVE_FUNC(Ipc_recfrom)(int* s, void *mem, size_t* len, int* flags, struct sockaddr* from, socklen_t* fromlen){
    return recvfrom(*s, mem, *len, *flags, from, fromlen);
}


int IPC_DO_SERVE_FUNC(Ipc_sendto)(int *s, const void *data, size_t *size, int *flags, const struct sockaddr *to, socklen_t *tolen){
    return sendto(*s, data, *size, *flags, to, *tolen);
}

int IPC_DO_SERVE_FUNC(Ipc_send)(int *s, const void *data, size_t *size, int *flags){
    return send(*s, data, *size, *flags);
}

int IPC_DO_SERVE_FUNC(Ipc_write)(int *s, const void *data, size_t *size){
    return write(*s, data, *size);
}

int IPC_DO_SERVE_FUNC(Ipc_close)(int *s){
    return close(*s);
}
int IPC_DO_SERVE_FUNC(Ipc_setsockopt)(int *s, int *level, int *optname, const void *optval, socklen_t *optlen){
    return setsockopt(*s, *level, *optname, optval, *optlen);
}


IPC_SERVER_INTERFACE(Ipc_socket, 3);
IPC_SERVER_INTERFACE(Ipc_bind, 3);
IPC_SERVER_INTERFACE(Ipc_connect, 3);
IPC_SERVER_INTERFACE(Ipc_listen, 2);
IPC_SERVER_INTERFACE(Ipc_accept, 3);
IPC_SERVER_INTERFACE(Ipc_read, 3);
IPC_SERVER_INTERFACE(Ipc_recv, 4);
IPC_SERVER_INTERFACE(Ipc_recfrom, 6);
IPC_SERVER_INTERFACE(Ipc_sendto, 6);
IPC_SERVER_INTERFACE(Ipc_send, 4);
IPC_SERVER_INTERFACE(Ipc_write, 3);
IPC_SERVER_INTERFACE(Ipc_close, 1);
IPC_SERVER_INTERFACE(Ipc_setsockopt, 5);

IPC_SERVER_REGISTER_INTERFACES(IpcLWIPServer, 12, 
        Ipc_socket, 
        Ipc_bind, 
        Ipc_connect,
        Ipc_listen, 
        Ipc_accept,
        Ipc_read, 
        Ipc_recv, 
        Ipc_recfrom, 
        Ipc_sendto, 
        Ipc_send, 
        Ipc_write, 
        Ipc_close,
        Ipc_setsockopt);

int main(int argc, char* argv[]){
    char lwip_ipaddr[4] = { 192, 168, 130, 77 };
    char lwip_netmask[4] = { 255, 255, 254, 0 };
    char lwip_gwaddr[4] = { 192, 168, 130, 1 };

    lwip_config_tcp(lwip_ipaddr, lwip_netmask, lwip_gwaddr);
    if (register_server("LWIPServer") < 0) {
        printf("register server name: %s failed.\n", "LWIPServer");
        exit(0);
    }
    ipc_server_loop(&IpcLWIPServer);
    // never reached
    exit(0);
}