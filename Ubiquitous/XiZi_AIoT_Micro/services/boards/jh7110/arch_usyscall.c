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
#include "usyscall.h"

int sbi_ecall(int ext, int fid, unsigned long arg0,
			unsigned long arg1, unsigned long arg2,
			unsigned long arg3, unsigned long arg4,
			unsigned long arg5)
{
	int ret;

	register uintptr_t a0 __asm__ ("a0") = (uintptr_t)(arg0);
	register uintptr_t a1 __asm__ ("a1") = (uintptr_t)(arg1);
	register uintptr_t a2 __asm__ ("a2") = (uintptr_t)(arg2);
	register uintptr_t a3 __asm__ ("a3") = (uintptr_t)(arg3);
	register uintptr_t a4 __asm__ ("a4") = (uintptr_t)(arg4);
	register uintptr_t a5 __asm__ ("a5") = (uintptr_t)(arg5);
	register uintptr_t a6 __asm__ ("a6") = (uintptr_t)(fid);
	register uintptr_t a7 __asm__ ("a7") = (uintptr_t)(ext);
	__asm__ volatile ("ecall"
		      : "+r" (a0), "+r" (a1)
		      : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
		      : "memory");
	ret = a0;

	return ret;
}

int syscall(int sys_num, intptr_t a1, intptr_t a2, intptr_t a3, intptr_t a4)
{
    int ret = -1;
    ret = sbi_ecall(sys_num, 0, a1, a2, a3, a4, 0, 0);
    return ret;
}
