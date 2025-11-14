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
#include "pingpong_service.h"
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

uintptr_t pingpong_sample(struct Session* session, int times)
{
    char* msg = "hello";
    uintptr_t tick1 = sys_test();
    for (int i = 0; i < times; i++) {
        uintptr_t tmp = sys_test();
    }
    uintptr_t tick2 = sys_test();
    uintptr_t time = (tick2 - tick1) / 24;
    // printf("Sync Tick Cost: %lu\n", time);
    return time;
}

uintptr_t pingpong_test_sync(struct Session* session, int times)
{
    char* msg = "hello";
    uintptr_t tick1 = sys_test();
    for (int i = 0; i < times; i++) {
        pingpong(session, msg);
    }
    uintptr_t tick2 = sys_test();
    uintptr_t time = (tick2 - tick1) / 24;
    // printf("Sync Tick Cost: %lu\n", time);
    return time;
}

uintptr_t pingpong_test_async(struct Session* session, int times)
{
    char* msg = "hello";
    uintptr_t tick1 = sys_test();
    for (int i = 0; i < times; i++) {
        pingpong_nowait(session, msg);
    }

    for (int i = 0; i < times; i++) {
        ipc_session_wait(session);
        ipc_session_forward(session);
    }
    uintptr_t tick2 = sys_test();
    uintptr_t time = (tick2 - tick1) / 24;
    // printf("ASync Tick Cost: %lu\n", (tick2 - tick1) / 24);
    return time;
}

int main(int argc, char** argv)
{
    int times = 0;
    if (argc >= 2) {
        times = string_to_integer(argv[1]);
    }

    struct Session session;
    if (connect_session(&session, "PingPongServer", 81920) < 0) {
        printf("connect session failed\n");
        exit(1);
    }

    printf("============ PingPong Test start ==============\n");
    uintptr_t loop = 50;
    uintptr_t t1 = 0, t2 = 0, t0 = 0;
    for (uintptr_t i = 0; i < loop; i++) {
        t0 += pingpong_sample(&session, times);
        t1 += pingpong_test_sync(&session, times);
        t2 += pingpong_test_async(&session, times);
    }

    printf("Compare Tick Cost: %lu\n", t0 / loop);
    printf("Sync Tick Cost: %lu\n", t1 / loop);
    printf("ASync Tick Cost: %lu\n", t2 / loop);
    printf("============= PingPong Test End ===============\n");

    exit(0);
}
