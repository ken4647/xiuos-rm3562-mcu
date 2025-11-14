// Copyright (c) 2006-2018 Frans Kaashoek, Robert Morris, Russ Cox, Massachusetts Institute of Technology

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/**
 * @file fs.c
 * @brief support read and write the fs.img
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */

/*************************************************
File name: fs.c
Description: support read and write the fs.img
Others: take ARM_XV6 kernel/fs.c for references
                https://github.com/KingofHamyang/ARM_xv6
History:
1. Date: 2024-01-25
Author: AIIT XUOS Lab
Modification:
1. remove inode lock and unlock
2. remove inode cache
3. rewrite skipelem function to PathElementExtract to fit XIZI_AIoT use sceneries
*************************************************/

#include <string.h>

#include "block_io.h"
#include "fs.h"
#include "libserial.h"

#define MIN_LENGTH(len1, len2) ((len1) < (len2) ? (len1) : (len2))

static int DirInodeAddEntry(struct Inode* dp, char* name, uint32_t inum);
static int DirInodeDelEntry(struct Inode* parent_inode, char* name);
static struct Inode* DirInodeLookup(struct Inode* dp, char* name);
static struct Inode* InodeAlloc(int type);
static int InodeFreeRecursive(struct Inode* dp);
static char* PathElementExtract(char* path, char* name);
static uint32_t InodeBlockMapping(struct Inode* inode, uint32_t block_num);

#define MAX_SUPPORT_FD 4096
static struct FileDescriptor fd_table[MAX_SUPPORT_FD];

struct MemFsRange MemFsRange;

/// @brief Using syscall to get fs.img real location in the memory
void MemFsInit(uintptr_t _binary_fs_img_start, uintptr_t fs_img_len)
{
    MemFsRange.memfs_start = _binary_fs_img_start;
    MemFsRange.memfs_nr_blocks = fs_img_len / BLOCK_SIZE;
}

/// @brief Read the super block.
void ReadSuperBlock(struct SuperBlock* sb)
{
    uint8_t* block = BlockRead(ROOT_INUM);
    memmove(sb, block, sizeof(*sb));
}

/// @brief Get a existed Inode by inum
struct Inode* InodeGet(uint32_t inum)
{
    struct Inode* inode;
    uint8_t* block = BlockRead(BLOCK_INDEX(inum));
    inode = (struct Inode*)block + INODE_INDEX(inum);
    return inode;
}

/// @brief Create a new Inode under the parent Inode
struct Inode* InodeCreate(struct Inode* parent_inode, char* name, int type)
{
    struct Inode* inode;
    if ((inode = DirInodeLookup(parent_inode, name)) != 0) {
        if (type == FS_FILE && inode->type == FS_FILE) {
            return inode;
        }
        return 0;
    }

    if ((inode = InodeAlloc(type)) == 0) {
        printf("InodeCreate: alloc Inode failed, no free inode\n");
        return 0;
    }

    if (type == FS_DIRECTORY) {
        if (DirInodeAddEntry(inode, ".", inode->inum) < 0 || DirInodeAddEntry(inode, "..", parent_inode->inum) < 0) {
            printf("InodeCreate: create dots");
            return 0;
        }
    }

    if (DirInodeAddEntry(parent_inode, name, inode->inum) < 0) {
        printf("InodeCreate: DirInodeAddEntry failed");
        return 0;
    }

    return inode;
}

/// @brief Delete a file Inode or a dir Inode
int InodeDelete(struct Inode* parent_inode, char* name)
{
    struct Inode* inode;

    if ((inode = DirInodeLookup(parent_inode, name)) == 0) {
        printf("Inode delete failed, file not exsit");
        return -1;
    }

    if (inode->type == FS_FILE) {
        inode->type = 0;
    } else if (inode->type == FS_DIRECTORY) {
        // recursive free alloced Inode
        if (InodeFreeRecursive(inode) < 0) {
            return -1;
        }
    }

    DirInodeDelEntry(parent_inode, name);
    return 0;
}

/// @brief Read data from the Inode to the dst buffer.
int InodeRead(struct Inode* inode, char* dst, int offset, int len)
{
    uint32_t location, writen_len;
    uint8_t* block;

    if (len < 0 || offset > inode->size) {
        return -1;
    }
    if (offset + len > inode->size) {
        len = inode->size - offset;
    }

    location = 0;
    while (location < len) {
        if ((block = BlockRead(InodeBlockMapping(inode, offset / BLOCK_SIZE))) == 0) {
            return 0;
        }
        writen_len = MIN_LENGTH(len - location, BLOCK_SIZE - offset % BLOCK_SIZE);
        memmove(dst, block + offset % BLOCK_SIZE, writen_len);
        location += writen_len;
        offset += writen_len;
        dst += writen_len;
    }

    return len;
}

/// @brief Write data from src buffer to the Inode, then increase the Inode size if neccessary.
int InodeWrite(struct Inode* inode, char* src, int offset, int len)
{
    uint32_t location, writen_len;
    uint8_t* block;

    if (len < 0 || offset > inode->size) {
        return -1;
    }
    if (offset + len > MAX_FILE_SIZE * BLOCK_SIZE) {
        return -1;
    }

    location = 0;
    while (location < len) {
        if ((block = BlockRead(InodeBlockMapping(inode, offset / BLOCK_SIZE))) == 0) {
            return 0;
        }
        writen_len = MIN_LENGTH(len - location, BLOCK_SIZE - offset % BLOCK_SIZE);
        memmove(block + offset % BLOCK_SIZE, src, writen_len);
        location += writen_len;
        offset += writen_len;
        src += writen_len;
    }

    if (len > 0 && offset > inode->size) {
        inode->size = offset;
    }

    return len;
}

/// @brief Find target Inode from source Inode
struct Inode* InodeSeek(struct Inode* source, char* path)
{
    if (source->size == 0) {
        printf("Inode is empty\n");
        return 0;
    }

    char name[DIR_NAME_SIZE] = { 0 };
    struct Inode *cur_inode, *next_inode;
    cur_inode = source;
    while ((path = PathElementExtract(path, name)) != 0) {
        if (cur_inode->type != FS_DIRECTORY) {
            return NULL;
        }
        if ((next_inode = DirInodeLookup(cur_inode, name)) == 0) {
            return NULL;
        }
        cur_inode = next_inode;
    }
    return cur_inode;
}

/// @brief Find target parent Inode from source Inode
struct Inode* InodeParentSeek(struct Inode* source, char* path, char* name)
{
    if (source->size == 0) {
        printf("Inode is empty\n");
        return 0;
    }

    struct Inode *cur_inode, *next_inode;
    cur_inode = source;
    while ((path = PathElementExtract(path, name)) != 0) {
        if (cur_inode->type != FS_DIRECTORY) {
            return NULL;
        }
        if (*path == '\0') {
            return cur_inode;
        }
        if ((next_inode = DirInodeLookup(cur_inode, name)) == 0) {
            return NULL;
        }
        cur_inode = next_inode;
    }

    return NULL;
}

/// @brief Alloc a new Inode using type
static struct Inode* InodeAlloc(int type)
{
    int inum = 0;
    struct Inode* inode;
    struct SuperBlock sb;

    ReadSuperBlock(&sb);
    for (inum = 1; inum < (int)sb.ninodes; inum++) {
        uint8_t* block = BlockRead(BLOCK_INDEX(inum));
        inode = (struct Inode*)block + INODE_INDEX(inum);
        if (inode->type == 0) {
            memset(inode, 0, sizeof(*inode));
            inode->inum = inum;
            inode->type = type;
            inode->size = 0;
            return inode;
        }
    }

    return NULL;
}

/// @brief Delete the dir and all files or dirs under the dir.
static int InodeFreeRecursive(struct Inode* parent_inode)
{
    uint32_t offset;
    struct Inode* inode;
    struct DirectEntry de;

    for (offset = 0; offset < parent_inode->size; offset += sizeof(de)) {
        if (InodeRead(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
            printf("inode_delete_dir failed: read directory entry failed");
            return -1;
        }

        if (de.inum == 0 || strcmp(de.name, "..") == 0 || strcmp(de.name, ".") == 0) {
            continue;
        }

        inode = InodeGet(de.inum);
        if (inode->type == FS_DIRECTORY) {
            if (InodeFreeRecursive(inode) < 0) {
                return -1;
            }
        } else if (inode->type == FS_FILE) {
            inode->type = 0;
        }

        // delete the dir entry
        de.inum = 0;
        if (InodeWrite(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
            printf("InodeDelete failed: clear directory entry failed");
            return -1;
        }
    }
    return 0;
}

/// @brief Mapping the direct block addrs or indirect block addrs of the Inode using the block_num
static uint32_t InodeBlockMapping(struct Inode* inode, uint32_t block_num)
{
    uint32_t addr;
    // block is in range of direct mapping
    if (block_num < NR_DIRECT_BLOCKS) {
        if ((addr = inode->addrs[block_num]) == 0) {
            inode->addrs[block_num] = addr = BlockAlloc();
        }
        return addr;
    }

    // alloc a new indirect indexing block
    block_num -= NR_DIRECT_BLOCKS;
    int indirect_block_id = block_num / MAX_INDIRECT_BLOCKS;
    if (indirect_block_id < NR_INDIRECT_BLOCKS) {
        if ((addr = inode->addrs[NR_DIRECT_BLOCKS + indirect_block_id]) == 0) {
            inode->addrs[NR_DIRECT_BLOCKS + indirect_block_id] = addr = BlockAlloc();
        }
        block_num -= indirect_block_id * MAX_INDIRECT_BLOCKS;
    } else {
        printf("InodeBlockMapping: out of range");
        return 0;
    }

    // alloc a new indirect block
    uint32_t* indirect_block = (uint32_t*)BlockRead(addr);
    if ((addr = indirect_block[block_num]) == 0) {
        indirect_block[block_num] = addr = BlockAlloc();
    }

    return addr;
}

/// @brief Look up the directory Inode for searching the target Inode
static struct Inode* DirInodeLookup(struct Inode* parent_inode, char* name)
{
    uint32_t offset, inum;
    struct DirectEntry de;

    if (parent_inode->type != FS_DIRECTORY) {
        printf("DirInodeLookup not DIR");
        return 0;
    }

    for (offset = 0; offset < parent_inode->size; offset += sizeof(de)) {
        if (InodeRead(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
            printf("DirInodeAddEntry read");
            return 0;
        }

        if (de.inum == 0) {
            continue;
        }

        if (strncmp((const char*)name, (const char*)de.name, DIR_NAME_SIZE) == 0) {
            inum = de.inum;
            return InodeGet(inum);
        }
    }

    return 0;
}

/// @brief Add a new directory entry for dir Inode
static int DirInodeAddEntry(struct Inode* parent_inode, char* name, uint32_t inum)
{
    int offset;
    struct DirectEntry de;
    struct Inode* inode;

    // Check the direct entry is not existed.
    if ((inode = DirInodeLookup(parent_inode, name)) != 0) {
        return -1;
    }

    // Look for an empty dir entry.
    for (offset = 0; offset < parent_inode->size; offset += sizeof(de)) {
        if (InodeRead(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
            printf("DirInodeAddEntry: read failed");
            return -1;
        }

        if (de.inum == 0) {
            break;
        }
    }

    // build a new direct entry.
    strncpy(de.name, name, DIR_NAME_SIZE);
    de.inum = inum;
    if (InodeWrite(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
        printf("DirInodeAddEntry: write failed");
        return -1;
    }

    return 0;
}

/// @brief Delete the directory entry for dir Inode
static int DirInodeDelEntry(struct Inode* parent_inode, char* name)
{
    int offset;
    struct DirectEntry de;
    struct Inode* inode;

    // Check the direct entry is existed.
    if ((inode = DirInodeLookup(parent_inode, name)) == 0) {
        return -1;
    }

    // Look for an empty dir entry.
    for (offset = 0; offset < parent_inode->size; offset += sizeof(de)) {
        if (InodeRead(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
            printf("DirInodeAddEntry: read failed");
            return -1;
        }

        if (strncmp(de.name, name, DIR_NAME_SIZE) == 0) {
            break;
        }
    }

    de.inum = 0;
    if (InodeWrite(parent_inode, (char*)&de, offset, sizeof(de)) != sizeof(de)) {
        printf("DirInodeAddEntry: write failed");
        return -1;
    }

    return 0;
}

// Paths process
static char* PathElementExtract(char* path, char* name)
{
    // Skip leading slashes
    while (*path == '/')
        path++;

    // Check for end of path
    if (*path == 0)
        return NULL;

    // Extract element
    char* start = path;
    while (*path != '/' && *path != 0)
        path++;

    // Calculate length and copy to 'name'
    int len = path - start;
    if (len >= DIR_NAME_SIZE)
        len = DIR_NAME_SIZE - 1;
    strncpy(name, start, len);
    name[len] = 0;

    // Skip trailing slashes
    while (*path == '/')
        path++;

    return path;
}

struct FileDescriptor* GetFileDescriptor(int fd)
{
    if (fd < 0 || fd > MAX_SUPPORT_FD) {
        printf("fd invlid.\n");
        return NULL;
    }
    return &fd_table[fd];
}

void FreeFileDescriptor(int fd)
{
    if (fd < 0 || fd > MAX_SUPPORT_FD) {
        printf("fd invlid.\n");
        return;
    }
    fd_table[fd].data = NULL;
    return;
}

int AllocFileDescriptor(void)
{
    int free_idx = -1;
    for (int i = 0; i < MAX_SUPPORT_FD; i++) {
        // found free fd
        if (free_idx == -1 && fd_table[i].data == NULL) {
            free_idx = i;
            break;
        }
    }
    if (free_idx == -1) {
        return -1;
    }
    return free_idx;
}
