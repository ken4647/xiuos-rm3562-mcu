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
 * @file mkfs.c
 * @brief make the user apps to the filesystem image
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024-01-25
 */

/*************************************************
File name: mkfs.c
Description: make the user apps to the filesystem image
Others: take ARM_XV6 tools/mkfs.c for references
                https://github.com/KingofHamyang/ARM_xv6
History:
1. Date: 2024-01-25
Author: AIIT XUOS Lab
Modification:
1. Increse the number of blocks and inodes
2. support more than one indirect blocks
3. remove unused stat and nlink property of inode struct
*************************************************/

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mkfs.h"

int fsfd;
struct SuperBlock sb;
char zeroes[BLOCK_SIZE];
uint freeblock;
uint usedblocks;
uint bitblocks;
uint freeinode = 1;

void balloc(int);
void wsect(uint, void*);
void winode(uint, struct Inode*);
void rinode(uint inum, struct Inode* ip);
void rsect(uint sec, void* buf);
uint ialloc(ushort type);
void iappend(uint inum, void* p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
    ushort y;
    uchar* a = (uchar*)&y;
    a[0] = x;
    a[1] = x >> 8;
    return y;
}

uint xint(uint x)
{
    uint y;
    uchar* a = (uchar*)&y;
    a[0] = x;
    a[1] = x >> 8;
    a[2] = x >> 16;
    a[3] = x >> 24;
    return y;
}

int main(int argc, char* argv[])
{
    int i, cc, fd;
    uint rootino, inum, off;
    struct DirEntry de;
    char buf[BLOCK_SIZE];
    struct Inode din;
    int size, nblocks;

    static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

    if (argc < 2) {
        fprintf(stderr, "Usage: mkfs fs.img files...\n");
        exit(1);
    }

    assert((BLOCK_SIZE % sizeof(struct Inode)) == 0);
    assert((BLOCK_SIZE % sizeof(struct DirEntry)) == 0);

    fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fsfd < 0) {
        perror(argv[1]);
        exit(1);
    }

    size = nr_blocks_total;
    bitblocks = NR_BIT_BLOCKS;
    usedblocks = 2 + (nr_inodes / INODE_SIZE + 1) + bitblocks;
    nblocks = nr_blocks_total - usedblocks;
    freeblock = usedblocks;

    sb.size = xint(size);
    sb.nblocks = xint(nblocks);
    sb.ninodes = xint(nr_inodes);
    sb.nbitblocks = xint(NR_BIT_BLOCKS);

    printf("block 0 is unused, block 1 is for super block, used %d (bit %d ninode %zu) free %u total %d\n", usedblocks,
        bitblocks, nr_inodes / INODE_SIZE + 1, size - usedblocks, nblocks + usedblocks);

    assert(nblocks + usedblocks == size);

    for (i = 0; i < nblocks + usedblocks; i++)
        wsect(i, zeroes);

    memset(buf, 0, sizeof(buf));
    memmove(buf, &sb, sizeof(sb));
    wsect(1, buf);

    // build root
    rootino = ialloc(FS_DIRECTORY);
    assert(rootino == ROOT_INUM);

    bzero(&de, sizeof(de));
    de.inum = xshort(rootino);
    strcpy(de.name, ".");
    iappend(rootino, &de, sizeof(de));

    bzero(&de, sizeof(de));
    de.inum = xshort(rootino);
    strcpy(de.name, "..");
    iappend(rootino, &de, sizeof(de));

    for (i = 2; i < argc; i++) {
        // assert(index(argv[i], '/') == 0);
        printf("add app: %s\n", argv[i]);

        if ((fd = open(argv[i], 0)) < 0) {
            perror(argv[i]);
            exit(1);
        }

        // Skip leading _ in name when writing to file system.
        // The binaries are named _rm, _cat, etc. to keep the
        // build operating system from trying to execute them
        // in place of system binaries like rm and cat.
        if (argv[i][0] == '_')
            ++argv[i];

        inum = ialloc(FS_FILE);

        bzero(&de, sizeof(de));
        de.inum = xshort(inum);
        strncpy(de.name, argv[i], DIR_NAME_SIZE);
        iappend(rootino, &de, sizeof(de));

        while ((cc = read(fd, buf, sizeof(buf))) > 0)
            iappend(inum, buf, cc);

        close(fd);
    }

    // fix size of root inode dir
    rinode(rootino, &din);
    off = xint(din.size);
    off = ((off / BLOCK_SIZE) + 1) * BLOCK_SIZE;
    din.size = xint(off);
    winode(rootino, &din);

    balloc(usedblocks);

    exit(0);
}

void wsect(uint sec, void* buf)
{
    if (lseek(fsfd, sec * BLOCK_SIZE, 0) != sec * BLOCK_SIZE) {
        perror("lseek");
        exit(1);
    }
    if (write(fsfd, buf, BLOCK_SIZE) != BLOCK_SIZE) {
        perror("write");
        exit(1);
    }
}

uint i2b(uint inum)
{
    return (inum / INODE_SIZE) + 2;
}

void winode(uint inum, struct Inode* ip)
{
    char buf[BLOCK_SIZE];
    uint bn;
    struct Inode* dip;

    bn = i2b(inum);
    rsect(bn, buf);
    dip = ((struct Inode*)buf) + (inum % INODE_SIZE);
    *dip = *ip;
    wsect(bn, buf);
}

void rinode(uint inum, struct Inode* ip)
{
    char buf[BLOCK_SIZE];
    uint bn;
    struct Inode* dip;

    bn = i2b(inum);
    rsect(bn, buf);
    dip = ((struct Inode*)buf) + (inum % INODE_SIZE);
    *ip = *dip;
}

void rsect(uint sec, void* buf)
{
    if (lseek(fsfd, sec * BLOCK_SIZE, 0) != sec * BLOCK_SIZE) {
        perror("lseek");
        exit(1);
    }
    if (read(fsfd, buf, BLOCK_SIZE) != BLOCK_SIZE) {
        perror("read");
        exit(1);
    }
}

uint ialloc(ushort type)
{
    uint inum = freeinode++;
    struct Inode din;

    bzero(&din, sizeof(din));
    din.type = xshort(type);
    din.size = xint(0);
    din.inum = inum;
    winode(inum, &din);
    return inum;
}

void balloc(int used)
{
    uchar buf[BLOCK_SIZE];
    int i;
    printf("balloc: first %d blocks have been allocated\n", used);
    for (int bmsec = 0; bmsec < bitblocks; bmsec++) {
        bzero(buf, BLOCK_SIZE);
        for (i = 0; i < ((used > NR_BIT_PER_BYTE * BLOCK_SIZE) ? NR_BIT_PER_BYTE * BLOCK_SIZE : used); i++) {
            buf[i / NR_BIT_PER_BYTE] = buf[i / NR_BIT_PER_BYTE] | (0x1 << (i % NR_BIT_PER_BYTE));
        }
        printf("balloc: write bitmap block at sector %zu\n", nr_inodes / INODE_SIZE + 3 + bmsec);
        wsect(nr_inodes / INODE_SIZE + 3 + bmsec, buf);
        used -= NR_BIT_PER_BYTE * BLOCK_SIZE;
    }
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void iappend(uint inum, void* xp, int n)
{
    char* p = (char*)xp;
    uint fbn, off, n1;
    struct Inode din;
    char buf[BLOCK_SIZE];
    uint indirect[NR_INDIRECT_BLOCKS * MAX_INDIRECT_BLOCKS];
    uint x;

    rinode(inum, &din);

    off = xint(din.size);
    while (n > 0) {
        fbn = off / BLOCK_SIZE;
        assert(fbn < MAX_FILE_SIZE);
        assert(usedblocks < nr_blocks_total);
        if (fbn < NR_DIRECT_BLOCKS) {
            if (xint(din.addrs[fbn]) == 0) {
                din.addrs[fbn] = xint(freeblock++);
                usedblocks++;
            }
            x = xint(din.addrs[fbn]);
        } else {
            int idx = fbn - NR_DIRECT_BLOCKS;
            int indirect_block_id = idx / MAX_INDIRECT_BLOCKS;
            if (indirect_block_id < NR_INDIRECT_BLOCKS) {
                if (xint(din.addrs[NR_DIRECT_BLOCKS + indirect_block_id]) == 0) {
                    // printf("allocate indirect block %d\n", indirect_block_id);
                    din.addrs[NR_DIRECT_BLOCKS + indirect_block_id] = xint(freeblock++);
                    usedblocks++;
                }
                // printf("read indirect block %d, idx is %d\n", indirect_block_id, idx);
                rsect(xint(din.addrs[NR_DIRECT_BLOCKS + indirect_block_id]), (char*)(indirect + MAX_INDIRECT_BLOCKS * indirect_block_id));
                if (indirect[idx] == 0) {
                    indirect[idx] = xint(freeblock++);
                    usedblocks++;
                    wsect(xint(din.addrs[NR_DIRECT_BLOCKS + indirect_block_id]), (char*)(indirect + MAX_INDIRECT_BLOCKS * indirect_block_id));
                }
                x = xint(indirect[idx]);
            }
        }
        n1 = min(n, (fbn + 1) * BLOCK_SIZE - off);
        rsect(x, buf);
        bcopy(p, buf + off - (fbn * BLOCK_SIZE), n1);
        wsect(x, buf);
        n -= n1;
        off += n1;
        p += n1;
    }
    din.size = xint(off);
    winode(inum, &din);
}
