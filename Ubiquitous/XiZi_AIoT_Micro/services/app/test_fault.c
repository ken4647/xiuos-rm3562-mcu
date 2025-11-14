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

// test_fs: Test the file io control of fs
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "libserial.h"
#include "usyscall.h"

int main(int argc, char* argv[])
{
    printf("Test memry error %s.\n", 0x50000000);
    printf("After error computing.\n");

    exit(0);
    return 0;
}
