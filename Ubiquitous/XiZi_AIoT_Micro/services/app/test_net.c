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

// test_net: Test the lwip network stack

#include "libserial.h"
#include "lwip_service.h"
#include "usyscall.h"

static char udp_ip_str[128] = {0};
static uint16_t udp_socket_port = 8888;
#define UDP_DEMO_SEND_TIMES      3

int main(int argc, char* argv[])
{
    printf("lwip network stack test \n");

    
    int cnt = UDP_DEMO_SEND_TIMES;
    char send_str[128];
    int fd = -1;

    memset(send_str, 0, sizeof(send_str));
    struct Session sess;
    connect_session(&sess, "LWIPServer", 4096);

    printf("%s %d\n", __func__, __LINE__);

    fd = ipc_socket(&sess, AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        printf("Socket error\n");
        return 0;
    }

    printf("%s %d\n", __func__, __LINE__);

    struct sockaddr_in udp_sock;
    udp_sock.sin_family = AF_INET;
    udp_sock.sin_port = htons(udp_socket_port);
    udp_sock.sin_addr.s_addr = inet_addr(udp_ip_str);
    memset(&(udp_sock.sin_zero), 0, sizeof(udp_sock.sin_zero));

    printf("%s %d\n", __func__, __LINE__);
    
    if(ipc_connect(&sess, fd, (struct sockaddr *)&udp_sock, sizeof(struct sockaddr)) < 0) {
        printf("Unable to connect %s:%d\n", udp_ip_str, udp_socket_port);
        ipc_close(&sess,fd);
        return 0;
    }

    printf("%s %d\n", __func__, __LINE__);

    printf("UDP connect %s:%d success, start to send.\n",
        udp_ip_str,
        udp_socket_port);

    while(cnt --) {
        snprintf(send_str, sizeof(send_str), "UDP test package times %d\r\n", cnt);
        ipc_send(&sess, fd, send_str, strlen(send_str), 0);
        printf("Send UDP msg: %s ", send_str);
    }

    ipc_close(&sess,fd);
    
    free_session(&sess);

    exit(0);
    return 0;
}