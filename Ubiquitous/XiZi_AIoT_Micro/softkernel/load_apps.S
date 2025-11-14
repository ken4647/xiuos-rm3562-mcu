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
 * @file load_apps.c
 * @brief embedded init memfs and memfs.img to kernel image
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: load_apps.c
Description: embedded init memfs and memfs.img to kernel image
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
.section .rawdata_fs_img
.globl user_apps
user_apps:
    .incbin "../services/app/fs.img"

.section .rawdata_init
.globl initapp
initapp:
    .incbin "../services/app/bin/shell"

.section .rawdata_memfs
.globl memfs
memfs:
    .incbin "../services/app/bin/fs_server"
.end

