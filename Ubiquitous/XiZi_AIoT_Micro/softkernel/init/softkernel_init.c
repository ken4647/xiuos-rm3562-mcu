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
 * @file softkernel_init.c
 * @brief init softkernel
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: softkernel_init.c
Description: init softkernel
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include "kern_init.h"

#include "assert.h"
#include "log.h"
#include "rbtree.h"
#include "task.h"

bool softkernel_init(TraceTag* _hardkernel_tag, struct TraceTag* _softkernel_tag)
{
    module_rbt_factory_init(_softkernel_tag);
    module_queue_factory_init(_softkernel_tag);

    struct TraceTag server_identifier_owner;
    CreateResourceTag(&server_identifier_owner, _softkernel_tag, "server-identifier", TRACER_OWNER, NULL);

    /* init soft kernel */
    struct PagerRightGroup pager_rights;
    AchieveResourceTag(&pager_rights.mmu_driver_tag, _hardkernel_tag, "mmu-ac-resource");
    module_pager_init(&pager_rights);

    struct TraceTag mmu_driver_tag, intr_driver_tag;
    AchieveResourceTag(&mmu_driver_tag, _hardkernel_tag, "mmu-ac-resource");
    AchieveResourceTag(&intr_driver_tag, _hardkernel_tag, "intr-ac-resource");
    load_kern_pgdir(&mmu_driver_tag, &intr_driver_tag); // enter kernel virtmem space

    module_task_manager_init(_softkernel_tag); // init task

    struct SharePageRightGroup sp_rights;
    AchieveResourceTag(&sp_rights.dcache_driver_tag, _hardkernel_tag, "dcache-ac-resource");
    AchieveResourceTag(&sp_rights.mmu_driver_tag, _hardkernel_tag, "mmu-ac-resource");
    module_share_page_init(&sp_rights);

    return true;
}

void show_xizi_bar(void)
{
    LOG_PRINTF("__  _____ ________     __  __ ___ ____ ____   ___\n");
    LOG_PRINTF("\\ \\/ /_ _|__  /_ _|   |  \\/  |_ _/ ___|  _ \\ / _ \\\n");
    LOG_PRINTF(" \\  / | |  / / | |    | |\\/| || | |   | |_) | | | |\n");
    LOG_PRINTF(" /  \\ | | / /_ | |    | |  | || | |___|  _ <| |_| |\n");
    LOG_PRINTF("/_/\\_\\___/____|___|   |_|  |_|___\\____|_| \\_\\\\___/ \n");
}