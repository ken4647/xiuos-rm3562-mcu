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

#pragma once

#include <stdint.h>

#include "fs.h"

// Block size
#define BLOCK_SIZE 512
#define NR_BIT_BLOCKS 8

// bits size
#define BITS 8

// Bitmap size of one block
#define BITMAP_SIZE (BLOCK_SIZE * BITS)

// Inode size of one block
#define INODE_SIZE (BLOCK_SIZE / sizeof(struct Inode))

// Caculate block bit index
#define BLOCK_BIT_INDEX(b, ninodes) ((b / BITMAP_SIZE) + (ninodes / INODE_SIZE) + 3)

// Caculate block index by inode number
#define BLOCK_INDEX(inum) ((inum / INODE_SIZE) + 2)

// Caculate inode index in the block
#define INODE_INDEX(inum) (inum % INODE_SIZE)

uint8_t* BlockRead(int block_num);
void BlockFree(int block_num);
uint32_t BlockAlloc();
