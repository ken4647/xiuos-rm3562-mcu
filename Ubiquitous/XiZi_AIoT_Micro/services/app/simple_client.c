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
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "libfs.h"
#include "libserial.h"
#include "simple_service.h"
#include "usyscall.h"

int string_to_integer(const char* str)
{
    assert(str);
    while (*str == ' ') {
        str++;
    }
    int is_positive = 1; // 默认是正
    if (*str - '+' == 0) {
        is_positive = 1;
        str++;
    }
    if (*str - '-' == 0) {
        is_positive = 0;
        str++;
    }
    long long result = 0; // int可能存不下 改为long long较为合适虽然返回值会丢失精度但感觉不要紧
    while ('0' <= *str && *str <= '9') {
        result *= 10;
        result += *str - '0'; // 如'1' ascii的值为 49 '0'的ascii的值为48 相减得到1

        if (result > INT32_MAX || result < INT32_MIN) // 可能出现的溢出问题
        {
            return result > INT32_MAX ? INT32_MAX : INT32_MIN;
        }

        str++;
    }
    return is_positive ? (result) : -result;
}

void itoa(int num, char* str, int radix)
{
    int i = 0;
    int sum;
    unsigned int num1 = num; // 如果是负数求补码，必须将他的绝对值放在无符号位中在进行求反码
    char str1[33] = { 0 };
    if (num < 0) { // 求出负数的补码
        num = -num;
        num1 = ~num;
        num1 += 1;
    }
    if (num == 0) {
        str1[i] = '0';

        i++;
    }
    while (num1 != 0) { // 进行进制运算
        sum = num1 % radix;
        str1[i] = (sum > 9) ? (sum - 10) + 'a' : sum + '0';
        num1 = num1 / radix;
        i++;
    }
    i--;
    int j = 0;
    for (i; i >= 0; i--) { // 逆序输出
        str[i] = str1[j];
        j++;
    }
}

int main(int argc, char** argv)
{
    int id = 1;
    if (argc >= 2) {
        id = string_to_integer(argv[1]);
    }

    struct Session session_wait;
    struct Session session_nowait;
    if (connect_session(&session_wait, "SimpleServer", 4096) < 0 || connect_session(&session_nowait, "SimpleServer", 4096) < 0) {
        printf("connect session failed\n");
        exit(1);
    }

    // test no wait ipc
    char *buf1 = NULL, *buf2 = NULL;
    struct IpcMsg* msg1 = hello_string_nowait(&session_nowait, &buf1, 32);
    struct IpcMsg* msg2 = hello_string_nowait(&session_nowait, &buf2, 128);

    // test ipc add(wait version)
    int ret = 0;
    ret = add(&session_wait, 17, 22);
    printf("ipc_add 17 + 22 = %d\n", ret);
    ret = add(&session_wait, 9, 9);
    printf("ipc_add 9 + 9 = %d\n", ret);

    struct Session fs_session;
    static char id_buf[33] = { 0 };
    if (id > 1) {
        if (connect_session(&fs_session, "MemFS", 0x4000) < 0) {
            printf("connect fs_session failed\n");
        } else {
            int fd;
            itoa(id - 1, id_buf, 10);
            char* shell_task_param[3] = { "/simple_client", id_buf, 0 };
            if ((fd = open(&fs_session, shell_task_param[0])) >= 0) {
                if (spawn(&fs_session, fd, read, fsize, shell_task_param[0], shell_task_param) < 0) {
                    printf("Syscall Spawn simple_client failed\n");
                }
                if (spawn(&fs_session, fd, read, fsize, shell_task_param[0], shell_task_param) < 0) {
                    printf("Syscall Spawn simple_client failed\n");
                }
                close(&fs_session, fd);
            } else {
                printf("Open %s failed\n", shell_task_param[0]);
            }
            free_session(&fs_session);
        }
    }

    ipc_session_wait(&session_nowait);
    printf("buf1: %s\n", buf1);
    ipc_session_forward(&session_nowait);

    ipc_session_wait(&session_nowait);
    printf("buf2: %s\n", buf2);
    ipc_session_forward(&session_nowait);

    free_session(&session_wait);
    free_session(&session_nowait);

    exit(0);
}
