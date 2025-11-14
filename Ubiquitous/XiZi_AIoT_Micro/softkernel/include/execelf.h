/* Copyright (c) 2006-2018 Frans Kaashoek, Robert Morris, Russ Cox,
 *                        Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/**
 * @file execelf.h
 * @brief Format of an ELF executable file
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.11.23
 */

/*************************************************
File name: execelf.h
Description: Format of an ELF executable file
Others:
History:
1. Date: 2023-11-23
Author: AIIT XUOS Lab
Modification:
1. reserve only necessary elfhdr structs
*************************************************/
#pragma once

#include <stdint.h>

#include "memlayout.h"

#define ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

#if (ARCH_BIT == 32)
// File header
struct elfhdr {
    uint32_t magic; // must equal ELF_MAGIC
    uint8_t elf[12];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uintptr_t entry;
    uintptr_t phoff;
    uintptr_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
};

// Program section header
struct proghdr {
    uint32_t type;
    uint32_t off;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
};
#elif (ARCH_BIT == 64)
struct elfhdr {
    uint32_t magic; // must equal ELF_MAGIC
    uint8_t elf[12];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
};

// Program section header
struct proghdr {
    uint32_t type;
    uint32_t flags;
    uint64_t off;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};
#endif

// Values for Proghdr type
#define ELF_PROG_LOAD 1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC 1
#define ELF_PROG_FLAG_WRITE 2
#define ELF_PROG_FLAG_READ 4
