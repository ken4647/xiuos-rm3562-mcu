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
 * @file fs.h
 * @brief file system important struct definition
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */

/*************************************************
File name: fs.h
Description: file system important struct definition
Others: take ARM_XV6 kernel/fs.h and kernel/file.h for references
                https://github.com/KingofHamyang/ARM_xv6
History:
1. Date: 2024-01-25
Author: AIIT XUOS Lab
Modification:
1. remove nlog member of superblock struct
2. rebuild inode struct to fit XIZI_AIoT use sceneries
3. change direct and indirect block number to fit XIZI_AIoT use sceneries
*************************************************/

#pragma once

#include <stdint.h>

#include "block_io.h"

// memory file system memlayout.
// [ block 0 ]  [  block 1  ]  [block 2] [...]  [  block 28  ]   [ ... ]
// [ unused  ]  [super block]  [Inode bit map]  [block bit map]  [data blocks]

#define NR_DIRECT_BLOCKS 4 // direct block number
#define NR_INDIRECT_BLOCKS 25 // indirect block number
#define ROOT_INUM 1 // root inode number

#define MAX_INDIRECT_BLOCKS (BLOCK_SIZE / sizeof(uint32_t)) // Mmaximum number of indirect blocks mapped per block
#define MAX_FILE_SIZE (NR_DIRECT_BLOCKS + (NR_INDIRECT_BLOCKS * MAX_INDIRECT_BLOCKS)) // maximum size of a file

// memory fs range
struct MemFsRange {
    uintptr_t memfs_start;
    uintptr_t memfs_nr_blocks;
};

// memfs file type
enum FILE_TYPE {
    FS_RESERVED = 0,
    FS_DIRECTORY, // Directory
    FS_FILE, // File
    FS_DEVICE, // Device
};

// File system super block
struct SuperBlock {
    uint32_t size; // Number of total blocks of file system image
    uint32_t nblocks; // Number of data blocks
    uint32_t ninodes; // Number of inodes.
    uint32_t nbitblocks;
};

// Inode structure
struct Inode {
    uint32_t inum; // Inode number
    uint32_t type; // File type
    uint32_t size; // Size of file (bytes)
    uint32_t addrs[NR_DIRECT_BLOCKS + NR_INDIRECT_BLOCKS]; // Data block addresses
};

// directory entry
#define DIR_NAME_SIZE 28
struct DirectEntry {
    uint32_t inum;
    char name[DIR_NAME_SIZE];
};

// file descriptor definition
#define MAX_PATH_LEN 128
struct FileDescriptor {
    char path[MAX_PATH_LEN];
    void* data;
};

// range of memory fs
extern struct MemFsRange MemFsRange;

void MemFsInit(uintptr_t _binary_fs_img_start, uintptr_t fs_img_len);
void ReadSuperBlock(struct SuperBlock*);

// fs Inode ops
struct Inode* InodeGet(uint32_t inum);
struct Inode* InodeCreate(struct Inode*, char*, int);
int InodeDelete(struct Inode*, char*);
int InodeRead(struct Inode*, char*, int, int);
int InodeWrite(struct Inode*, char*, int, int);
struct Inode* InodeSeek(struct Inode*, char*);
struct Inode* InodeParentSeek(struct Inode*, char*, char*);

// fs fd ops
struct FileDescriptor* GetFileDescriptor(int fd);
void FreeFileDescriptor(int fd);
int AllocFileDescriptor(void);
