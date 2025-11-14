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
 * @file error_debug.c
 * @brief handle program abort
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2024.4.25
 */

/*************************************************
File name: error_debug.c
Description: handle program abort
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. Take only armv8 abort reason part(_abort_reason).
2. Modify iabort and dabort handler(in dabort_handler() and iabort_handler())
*************************************************/
#include <stddef.h>
#include <stdint.h>

#include "assert.h"
#include "core.h"
#include "log.h"
#include "multicores.h"
#include "task.h"
#include "trap_common.h"

void dump_tf(struct trapframe* tf)
{

}

void dabort_reason(struct trapframe* r)
{

    return;
}

void iabort_reason(struct trapframe* r)
{
    return;
}