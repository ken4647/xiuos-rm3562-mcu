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
#include <stdbool.h>
#include <string.h>

#include "libipc.h"

IPC_SERVICES(IpcFsServer, Ipc_ls, Ipc_cd, Ipc_mkdir, Ipc_delete, Ipc_cat,
    Ipc_open, Ipc_close, Ipc_read, Ipc_write, Ipc_fsize);

int ls(struct Session* session, char* path);
int cd(struct Session* session, char* path);
int mkdir(struct Session* session, char* path);
int rm(struct Session* session, char* path);
int cat(struct Session* session, char* path);
int fsize(struct Session* session, int fd);

int open(struct Session* session, char* path);
int close(struct Session* session, int fd);
int read(struct Session* session, int fd, char* dst, int offset, int len);
int write(struct Session* session, int fd, char* src, int offset, int len);

#define FS_IMG_ADDR 0x60000000
