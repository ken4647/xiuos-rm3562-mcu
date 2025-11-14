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
 * @brief inode and block declaration
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */

/*************************************************
File name: fs.h
Description: inode and block declaration
Others: take ARM_XV6 include/fs.h and include/stat.h for references
                https://github.com/KingofHamyang/ARM_xv6
History:
1. Date: 2024-01-25
Author: AIIT XUOS Lab
Modification:
1. Increse the number of indirect blocks
2. Remove unused nlink of inode struct
*************************************************/

#pragma once

#define FS_DIRECTORY 1 // Directory
#define FS_FILE 2 // File
#define FS_DEVICE 3 // Device

#define ROOT_INUM 1 // root inode number
#define BLOCK_SIZE 512 // block size
#define NR_BIT_PER_BYTE 8
#define NR_BIT_BLOCKS 8
#define nr_blocks_total (BLOCK_SIZE * NR_BIT_PER_BYTE * NR_BIT_BLOCKS) // total number of blocks (including used blocks and free blocks)
#define nr_inodes 200 // total number of inodes

#define NR_DIRECT_BLOCKS 4
#define NR_INDIRECT_BLOCKS 25
#define MAX_INDIRECT_BLOCKS (BLOCK_SIZE / sizeof(uint))

#define MAX_FILE_SIZE (NR_DIRECT_BLOCKS + (NR_INDIRECT_BLOCKS * MAX_INDIRECT_BLOCKS))

// Inode size per block
#define INODE_SIZE (BLOCK_SIZE / sizeof(struct Inode))

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

// File system super block
struct SuperBlock {
    uint size; // Size of file system image (blocks)
    uint nblocks; // Number of data blocks
    uint ninodes; // Number of inodes.
    uint nbitblocks;
};

// Inode structure
struct Inode {
    uint inum; // inode number
    uint type; // File type
    uint size; // Size of file (bytes)
    uint addrs[NR_DIRECT_BLOCKS + NR_INDIRECT_BLOCKS]; // Data block addresses
};

// Directory is a file containing a sequence of DirEntry structures.
#define DIR_NAME_SIZE 28
struct DirEntry {
    __uint32_t inum;
    char name[DIR_NAME_SIZE];
};
