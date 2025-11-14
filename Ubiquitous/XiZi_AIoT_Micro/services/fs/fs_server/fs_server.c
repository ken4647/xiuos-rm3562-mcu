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

#include "block_io.h"
#include "fs.h"
#include "libfs.h"
#include "libserial.h"
#include "usyscall.h"

struct CwdPair {
    int session_id;
    struct Inode* Inode;
};

#define MAX_SUPPORT_SESSION 4096
static struct CwdPair session_cwd[MAX_SUPPORT_SESSION];

static struct CwdPair* get_session_cwd(void)
{
    int free_idx = -1;
    for (int i = 0; i < MAX_SUPPORT_SESSION; i++) {
        // found session cwd
        if (session_cwd[i].session_id == cur_session_id()) {
            return &session_cwd[i];
        }
        if (free_idx == -1 && session_cwd[i].session_id == 0) {
            free_idx = i;
        }
    }
    // new a cwd pair for session
    if (free_idx == -1) {
        printf("Connect Session reaches max value.\n");
        return NULL;
    }
    session_cwd[free_idx].session_id = cur_session_id();
    session_cwd[free_idx].Inode = InodeGet(ROOT_INUM);
    return &session_cwd[free_idx];
}

int IPC_DO_SERVE_FUNC(Ipc_ls)(char* path)
{
    struct Inode *dp, *ip;
    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        struct CwdPair* cwd = get_session_cwd();
        if (!cwd) {
            printf("ls:find current work dir failed\n");
            return -1;
        }
        dp = cwd->Inode;
    }

    if (!(ip = InodeSeek(dp, path))) {
        printf("ls: find target Inode failed, dp: %p(%d), path: %s\n", dp, dp->inum, path);
        return -1;
    }

    if (ip->type != FS_DIRECTORY) {
        printf("ls: not a dir, ip: %d\n", ip->inum);
        return -1;
    }

    uint32_t off;
    struct DirectEntry de;
    for (off = 0; off < ip->size; off += sizeof(de)) {
        if (InodeRead(ip, (char*)&de, off, sizeof(de)) != sizeof(de)) {
            printf("ls:read dir entry failed\n");
            return -1;
        }
        if (de.inum == 0) {
            continue;
        }
        printf("%s\n", de.name);
    }

    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_cd)(char* path)
{
    struct CwdPair* cwd;
    struct Inode *dp, *ip;

    cwd = get_session_cwd();
    if (!cwd) {
        printf("cd:find current work dir failed\n");
        return -1;
    }

    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        dp = cwd->Inode;
    }

    if (!(ip = InodeSeek(dp, path))) {
        /// @todo Is need to create the Inode when the dir is node existed?
        printf("cd:find target Inode failed\n");
        return -1;
    }

    if (ip->type != FS_DIRECTORY) {
        printf("cd:not a dir\n");
        return -1;
    }

    cwd->Inode = ip;

    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_mkdir)(char* path)
{
    // printf("Create a new directory: %s\n", path);
    struct Inode *dp, *ip;
    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        struct CwdPair* cwd = get_session_cwd();
        if (!cwd) {
            printf("find current work dir failed\n");
            return -1;
        }
        dp = cwd->Inode;
    }

    if ((ip = InodeSeek(dp, path)) != NULL) {
        /// @todo Is need to return target Inode?
        printf("target Inode existed\n");
        return -1;
    }

    char name[DIR_NAME_SIZE] = { 0 };
    if (!(ip = InodeParentSeek(dp, path, name))) {
        /// @todo Is need to return target Inode?
        printf("target parent Inode is not existed\n");
        return -1;
    }

    if (InodeCreate(ip, name, FS_DIRECTORY) == 0) {
        printf("create target Inode %s failed\n", path);
        return -1;
    }

    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_delete)(char* path)
{
    struct Inode *dp, *ip;
    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        struct CwdPair* cwd = get_session_cwd();
        if (!cwd) {
            printf("delete:find current work dir failed\n");
            return -1;
        }
        dp = cwd->Inode;
    }

    char name[DIR_NAME_SIZE] = { 0 };
    if (!(ip = InodeParentSeek(dp, path, name))) {
        printf("delete:target file parent not existed\n");
        return -1;
    }

    if (InodeDelete(ip, name) < 0) {
        printf("delete:target file not existed\n");
        return -1;
    }

    return 0;
}

/// @todo Support malloc for user
int IPC_DO_SERVE_FUNC(Ipc_cat)(char* path)
{
    static char buffer[BLOCK_SIZE + 1] = { 0 };
    buffer[BLOCK_SIZE] = '\0';

    struct Inode *dp, *ip;
    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        struct CwdPair* cwd = get_session_cwd();
        if (!cwd) {
            printf("delete:find current work dir failed\n");
            return -1;
        }
        dp = cwd->Inode;
    }

    if (!(ip = InodeSeek(dp, path))) {
        printf("delete:target file not existed\n");
        return -1;
    }

    if (ip->type != FS_FILE) {
        printf("cat: %s Is not a file\n", path);
        return -1;
    }

    uint32_t off;
    for (off = 0; off < ip->size; off += BLOCK_SIZE) {
        if (InodeRead(ip, buffer, off, BLOCK_SIZE) < 0) {
            printf("cat: read file data failed\n");
            break;
        }
        printf("%s", buffer);
    }

    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_open)(char* path)
{
    // printf("Ipc_open: %s\n", path);
    int fd;
    struct FileDescriptor* fdp;
    fd = AllocFileDescriptor();
    if (fd < 0) {
        printf("open: alloc a new fd failed\n");
        return -1;
    }
    fdp = GetFileDescriptor(fd);

    struct Inode *dp, *ip;
    if (*path == '/') {
        dp = InodeGet(ROOT_INUM);
    } else {
        struct CwdPair* cwd = get_session_cwd();
        if (!cwd) {
            printf("ls:find current work dir failed\n");
            return -1;
        }
        dp = cwd->Inode;
    }
    if ((ip = InodeSeek(dp, path)) == NULL) {
        printf("open: find target Inode failed, path is %s\n", path);
        return -1;
    }

    /// @todo record absolute path
    strncpy(fdp->path, path, strlen(path) + 1);
    fdp->data = ip;

    return fd;
}

int IPC_DO_SERVE_FUNC(Ipc_close)(int* fd)
{
    FreeFileDescriptor(*fd);
    return 0;
}

int IPC_DO_SERVE_FUNC(Ipc_read)(int* fd, char* dst, int* offset, int* len)
{
    // printf("Ipc_read, fd is %d, dst: %x, offset: %d, len: %d\n", *fd, dst, *offset, *len);
    struct FileDescriptor* fdp = GetFileDescriptor(*fd);
    if (!fdp) {
        printf("read: fd invalid\n");
        return -1;
    }

    struct Inode* ip = fdp->data;
    if (ip->type != FS_FILE) {
        printf("read: %s Is not a file\n", fdp->path);
        return -1;
    }

    int cur_read_len = InodeRead(ip, dst, *offset, *len);

    return cur_read_len;
}

int IPC_DO_SERVE_FUNC(Ipc_write)(int* fd, char* src, int* offset, int* len)
{
    // printf("Ipc_write, fd is %d\n", *fd);
    struct FileDescriptor* fdp = GetFileDescriptor(*fd);
    if (!fdp) {
        printf("read: fd invalid\n");
        return -1;
    }

    struct Inode* ip = fdp->data;
    if (ip->type != FS_FILE) {
        printf("read: %s Is not a file\n", fdp->path);
        return -1;
    }

    int cur_write_len = InodeWrite(ip, src, *offset, *len);

    return cur_write_len;
}

int IPC_DO_SERVE_FUNC(Ipc_fsize)(int* fd)
{
    struct FileDescriptor* fdp = GetFileDescriptor(*fd);
    if (!fdp) {
        printf("read: fd invalid\n");
        return -1;
    }

    struct Inode* ip = fdp->data;
    if (ip->type != FS_FILE) {
        printf("read: %s Is not a file\n", fdp->path);
        return -1;
    }

    return ip->size;
}

IPC_SERVER_INTERFACE(Ipc_ls, 1);
IPC_SERVER_INTERFACE(Ipc_cd, 1);
IPC_SERVER_INTERFACE(Ipc_mkdir, 1);
IPC_SERVER_INTERFACE(Ipc_delete, 1);
IPC_SERVER_INTERFACE(Ipc_cat, 1);
IPC_SERVER_INTERFACE(Ipc_fsize, 1);

IPC_SERVER_INTERFACE(Ipc_open, 1);
IPC_SERVER_INTERFACE(Ipc_close, 1);
IPC_SERVER_INTERFACE(Ipc_read, 4);
IPC_SERVER_INTERFACE(Ipc_write, 4);

IPC_SERVER_REGISTER_INTERFACES(IpcFsServer, 10,
    Ipc_ls,
    Ipc_cd,
    Ipc_mkdir,
    Ipc_delete,
    Ipc_cat,
    Ipc_open,
    Ipc_close,
    Ipc_read,
    Ipc_write,
    Ipc_fsize);

int main(int argc, char* argv[])
{
    sys_state_info info;
    get_memblock_info(&info);

    uintptr_t len = info.memblock_info.memblock_end - info.memblock_info.memblock_start;
    mmap(FS_IMG_ADDR, info.memblock_info.memblock_start, len, false);

    MemFsInit((uintptr_t)FS_IMG_ADDR, (uintptr_t)len);

    if (register_server("MemFS") < 0) {
        printf("register server name: %s failed.\n", "MemFs");
        exit(1);
    }
    ipc_server_loop(&IpcFsServer);

    // never reached
    exit(0);
    return 0;
}
