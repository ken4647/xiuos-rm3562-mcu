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

#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include <lwip/opt.h>
#include <lwip/arch.h>
#include <lwip/netif.h>
// #include "tcpip.h"

#include "libsemaphore.h"
#include "libipc.h"

/* LWIP task parameter */
#define LWIP_LOCAL_PORT             4840
#define LWIP_TARGET_PORT            LWIP_LOCAL_PORT

#define LWIP_DEMO_TIMES             10
#define LWIP_TASK_STACK_SIZE        4096
#define LWIP_DEMO_TASK_PRIO         20

// /* MAC address configuration. */
// #define configMAC_ADDR {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}
// #define configMAC_ADDR_ETH1 {0x02, 0x12, 0x13, 0x10, 0x15, 0x12}

/* USER CODE END 0 */
#define SYS_MBOX_NULL  NULL
#define SYS_SEM_NULL   NULL
#define SYS_MRTEX_NULL SYS_SEM_NULL
#define SYS_MBOX_SIZE 128


typedef int sys_sem_t;
typedef int sys_mutex_t;


struct sys_mbox{
    int first, last;
    void *msgs[SYS_MBOX_SIZE];
    int not_empty;
    int not_full;
    int mutex;
    int wait_send;
    int valid;
};
typedef struct sys_mbox sys_mbox_t;

typedef int sys_thread_t;
typedef s64_t sys_prot_t;

#define MS_PER_SYSTICK (float)(1000 / TICK_PER_SECOND)
#define TICKS_PER_MS (TICK_PER_SECOND / 1000)

int lwip_config_tcp(char *ip, char *mask, char *gw);


#endif /* LWIP_ARCH_SYS_ARCH_H */

