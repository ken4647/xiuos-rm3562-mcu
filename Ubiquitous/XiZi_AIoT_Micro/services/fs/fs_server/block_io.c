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

#include <string.h>

#include "block_io.h"
#include "libserial.h"
#include "fs.h"
#include "list.h"

static void Error(char* s)
{
    printf("Error: %s\n", s);
    for (;;)
        ;
}

// Locate the data block using block_num
uint8_t* BlockRead(int block_num)
{
    uint8_t* data = (uint8_t*)MemFsRange.memfs_start + block_num * BLOCK_SIZE;
    return data;
}

// Zero a block.
static void BlockClear(int block_num)
{
    uint8_t* data = BlockRead(block_num);
    memset(data, 0, BLOCK_SIZE);
}

// Allocate a zeroed disk block.
uint32_t BlockAlloc()
{
    int bit_index, block_index, bit_mark;

    struct SuperBlock sb;

    ReadSuperBlock(&sb);

    for (bit_index = 0; bit_index < sb.size; bit_index += BITMAP_SIZE) {

        uint8_t* data = BlockRead(BLOCK_BIT_INDEX(bit_index, sb.ninodes));

        for (block_index = 0; block_index < BITMAP_SIZE && bit_index + block_index < sb.size; block_index++) {
            bit_mark = 1 << (block_index % 8);

            if ((data[block_index / 8] & bit_mark) == 0) {
                data[block_index / 8] |= bit_mark;

                BlockClear(bit_index + block_index);
                return bit_index + block_index;
            }
        }
    }

    printf("BlockAlloc: out of blocks, bit idx: %d, total size: %d\n", bit_index, sb.size);
    Error("");
    return 0;
}

// Free a disk block.
void BlockFree(int block_num)
{
    struct SuperBlock sb;
    int block_index, bit_mark;

    ReadSuperBlock(&sb);

    uint8_t* data = BlockRead(BLOCK_BIT_INDEX(block_num, sb.ninodes));

    block_index = block_num % BITMAP_SIZE;
    bit_mark = 1 << (block_index % 8);

    if ((data[block_index / 8] & bit_mark) == 0) {
        Error("freeing free block");
    }

    data[block_index / 8] &= ~bit_mark;
}
