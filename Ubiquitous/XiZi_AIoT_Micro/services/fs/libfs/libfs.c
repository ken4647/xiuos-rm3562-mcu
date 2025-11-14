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
#include "libfs.h"

IPC_INTERFACE(Ipc_ls, 1, path, strlen(path) + 1);
int ls(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_ls)(session, path);
}

IPC_INTERFACE(Ipc_cd, 1, path, strlen(path) + 1);
int cd(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_cd)(session, path);
}

IPC_INTERFACE(Ipc_mkdir, 1, path, strlen(path) + 1);
int mkdir(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_mkdir)(session, path);
}

IPC_INTERFACE(Ipc_delete, 1, path, strlen(path) + 1);
int rm(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_delete)(session, path);
}

IPC_INTERFACE(Ipc_cat, 1, path, strlen(path) + 1);
int cat(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_cat)(session, path);
}

IPC_INTERFACE(Ipc_open, 1, path, strlen(path) + 1);
int open(struct Session* session, char* path)
{
    return IPC_CALL(Ipc_open)(session, path);
}

IPC_INTERFACE(Ipc_close, 1, fd, sizeof(int));
int close(struct Session* session, int fd)
{
    return IPC_CALL(Ipc_close)(session, &fd);
}

IPC_INTERFACE(Ipc_read, 4, fd, dst, offset, len, sizeof(int), *(int*)len, sizeof(int), sizeof(int));
int read(struct Session* session, int fd, char* dst, int offset, int len)
{
    return IPC_CALL(Ipc_read)(session, &fd, dst, &offset, &len);
}

IPC_INTERFACE(Ipc_write, 4, fd, src, offset, len, sizeof(int), *(int*)len, sizeof(int), sizeof(int));
int write(struct Session* session, int fd, char* src, int offset, int len)
{
    return IPC_CALL(Ipc_write)(session, &fd, src, &offset, &len);
}

IPC_INTERFACE(Ipc_fsize, 1, fd, sizeof(int));
int fsize(struct Session* session, int fd)
{
    return IPC_CALL(Ipc_fsize)(session, &fd);
}