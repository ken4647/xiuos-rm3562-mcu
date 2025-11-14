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
/**
 * @file ipcargs.c
 * @brief macros to supprt libipc
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: ipcargs.c
Description: macros to supprt libipc
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#define ARG(x) (x)
#define DEAULT_VA_NUM 12
#define _VA_NUM(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, N, M, ...) ((N == M + 1) ? ((N == 1) ? ((#x0)[0] != 0) : N) : DEAULT_VA_NUM)
#define VA_NUM(...) ARG(_VA_NUM(__VA_ARGS__, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1))

#define _VA_ARG0(x0, ...) (x0)
#define _VA_ARG1(x0, x1, ...) (x1)
#define _VA_ARG2(x0, x1, x2, ...) (x2)
#define _VA_ARG3(x0, x1, x2, x3, ...) (x3)
#define _VA_ARG4(x0, x1, x2, x3, x4, ...) (x4)
#define _VA_ARG5(x0, x1, x2, x3, x4, x5, ...) (x5)
#define _VA_ARG6(x0, x1, x2, x3, x4, x5, x6, ...) (x6)
#define _VA_ARG7(x0, x1, x2, x3, x4, x5, x6, x7, ...) (x7)
#define _VA_ARG8(x0, x1, x2, x3, x4, x5, x6, x7, x8, ...) (x8)
#define _VA_ARG9(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) (x9)
#define _VA_ARG10(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ...) (x10)
#define _VA_ARG11(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ...) (x11)

#define _VA_ARG0_WO(x0, ...) x0
#define _VA_ARG1_WO(x0, x1, ...) x1
#define _VA_ARG2_WO(x0, x1, x2, ...) x2
#define _VA_ARG3_WO(x0, x1, x2, x3, ...) x3
#define _VA_ARG4_WO(x0, x1, x2, x3, x4, ...) x4
#define _VA_ARG5_WO(x0, x1, x2, x3, x4, x5, ...) x5
#define _VA_ARG6_WO(x0, x1, x2, x3, x4, x5, x6, ...) x6
#define _VA_ARG7_WO(x0, x1, x2, x3, x4, x5, x6, x7, ...) x7
#define _VA_ARG8_WO(x0, x1, x2, x3, x4, x5, x6, x7, x8, ...) x8
#define _VA_ARG9_WO(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) x9
#define _VA_ARG10_WO(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ...) x10
#define _VA_ARG11_WO(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ...) x11

#define _VA_FRONT_ARG1(...) _VA_ARG0_WO(__VA_ARGS__)
#define _VA_FRONT_ARG2(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__)
#define _VA_FRONT_ARG3(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__)
#define _VA_FRONT_ARG4(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__)
#define _VA_FRONT_ARG5(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__)
#define _VA_FRONT_ARG6(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__)
#define _VA_FRONT_ARG7(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__), _VA_ARG6_WO(__VA_ARGS__)
#define _VA_FRONT_ARG8(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__), _VA_ARG6_WO(__VA_ARGS__), _VA_ARG7_WO(__VA_ARGS__)
#define _VA_FRONT_ARG9(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__), _VA_ARG6_WO(__VA_ARGS__), _VA_ARG7_WO(__VA_ARGS__), _VA_ARG8_WO(__VA_ARGS__)
#define _VA_FRONT_ARG10(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__), _VA_ARG6_WO(__VA_ARGS__), _VA_ARG7_WO(__VA_ARGS__), _VA_ARG8_WO(__VA_ARGS__), _VA_ARG9_WO(__VA_ARGS__)
#define _VA_FRONT_ARG11(...) _VA_ARG0_WO(__VA_ARGS__), _VA_ARG1_WO(__VA_ARGS__), _VA_ARG2_WO(__VA_ARGS__), _VA_ARG3_WO(__VA_ARGS__), _VA_ARG4_WO(__VA_ARGS__), _VA_ARG5_WO(__VA_ARGS__), _VA_ARG6_WO(__VA_ARGS__), _VA_ARG7_WO(__VA_ARGS__), _VA_ARG8_WO(__VA_ARGS__), _VA_ARG9_WO(__VA_ARGS__), _VA_ARG10_WO(__VA_ARGS__)

#define _VA_ARG0_PTR(x0, ...) void* x0
#define _VA_ARG1_PTR(x0, x1, ...) void* x1
#define _VA_ARG2_PTR(x0, x1, x2, ...) void* x2
#define _VA_ARG3_PTR(x0, x1, x2, x3, ...) void* x3
#define _VA_ARG4_PTR(x0, x1, x2, x3, x4, ...) void* x4
#define _VA_ARG5_PTR(x0, x1, x2, x3, x4, x5, ...) void* x5
#define _VA_ARG6_PTR(x0, x1, x2, x3, x4, x5, x6, ...) void* x6
#define _VA_ARG7_PTR(x0, x1, x2, x3, x4, x5, x6, x7, ...) void* x7
#define _VA_ARG8_PTR(x0, x1, x2, x3, x4, x5, x6, x7, x8, ...) void* x8
#define _VA_ARG9_PTR(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) void* x9
#define _VA_ARG10_PTR(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ...) void* x10
#define _VA_ARG11_PTR(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ...) void* x11

#define _VA_FRONT_PTR_ARG1(...) _VA_ARG0_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG2(...) _VA_FRONT_PTR_ARG1(__VA_ARGS__), _VA_ARG1_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG3(...) _VA_FRONT_PTR_ARG2(__VA_ARGS__), _VA_ARG2_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG4(...) _VA_FRONT_PTR_ARG3(__VA_ARGS__), _VA_ARG3_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG5(...) _VA_FRONT_PTR_ARG4(__VA_ARGS__), _VA_ARG4_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG6(...) _VA_FRONT_PTR_ARG5(__VA_ARGS__), _VA_ARG5_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG7(...) _VA_FRONT_PTR_ARG6(__VA_ARGS__), _VA_ARG6_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG8(...) _VA_FRONT_PTR_ARG7(__VA_ARGS__), _VA_ARG7_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG9(...) _VA_FRONT_PTR_ARG8(__VA_ARGS__), _VA_ARG8_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG10(...) _VA_FRONT_PTR_ARG9(__VA_ARGS__), _VA_ARG9_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG11(...) _VA_FRONT_PTR_ARG10(__VA_ARGS__), _VA_ARG10_PTR(__VA_ARGS__)
#define _VA_FRONT_PTR_ARG12(...) _VA_FRONT_PTR_ARG11(__VA_ARGS__), _VA_ARG11_PTR(__VA_ARGS__)

#define _VA_ARGS_SECOND(x1, ...) __VA_ARGS__
#define _VA_WRAP_INNER(wrap, x, ...) wrap x
#define _VA_FRONT_WRAP_ARG1(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG2(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG1(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG3(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG2(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG4(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG3(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG5(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG4(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG6(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG5(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG7(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG6(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG8(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG7(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG9(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG8(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG10(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG9(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG11(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG10(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG12(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG11(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG13(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG12(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG14(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG13(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG15(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG14(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG16(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG15(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG17(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG16(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG18(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG17(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG19(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG18(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG20(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG19(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG21(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG20(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG22(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG21(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG23(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG22(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG24(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG23(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG25(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG24(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG26(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG25(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG27(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG26(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG28(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG27(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG29(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG28(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG30(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG29(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG31(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG30(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG32(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG31(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG33(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG32(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG34(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG33(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG35(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG34(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG36(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG35(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG37(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG36(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG38(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG37(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG39(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG38(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG40(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG39(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG41(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG40(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG42(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG41(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG43(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG42(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG44(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG43(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG45(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG44(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG46(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG45(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG47(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG46(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG48(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG47(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG49(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG48(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG50(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG49(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG51(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG50(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG52(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG51(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG53(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG52(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG54(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG53(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG55(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG54(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG56(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG55(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG57(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG56(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG58(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG57(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG59(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG58(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG60(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG59(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG61(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG60(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG62(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG61(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG63(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG62(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG64(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG63(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG65(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG64(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG66(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG65(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG67(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG66(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG68(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG67(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG69(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG68(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG70(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG69(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG71(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG70(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG72(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG71(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG73(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG72(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG74(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG73(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG75(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG74(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG76(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG75(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG77(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG76(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG78(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG77(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG79(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG78(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG80(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG79(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG81(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG80(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG82(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG81(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG83(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG82(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG84(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG83(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG85(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG84(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG86(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG85(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG87(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG86(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG88(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG87(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG89(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG88(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG90(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG89(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG91(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG90(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG92(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG91(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG93(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG92(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG94(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG93(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG95(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG94(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG96(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG95(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG97(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG96(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG98(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG97(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG99(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG98(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG100(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG99(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG101(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG100(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG102(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG101(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG103(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG102(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG104(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG103(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG105(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG104(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG106(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG105(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG107(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG106(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG108(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG107(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG109(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG108(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG110(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG109(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG111(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG110(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG112(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG111(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG113(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG112(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG114(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG113(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG115(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG114(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG116(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG115(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG117(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG116(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG118(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG117(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG119(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG118(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG120(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG119(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG121(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG120(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG122(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG121(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG123(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG122(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG124(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG123(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG125(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG124(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG126(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG125(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG127(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG126(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG128(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG127(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG129(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG128(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG130(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG129(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG131(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG130(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG132(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG131(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG133(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG132(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG134(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG133(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG135(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG134(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG136(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG135(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG137(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG136(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG138(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG137(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG139(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG138(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG140(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG139(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG141(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG140(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG142(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG141(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG143(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG142(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG144(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG143(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG145(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG144(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG146(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG145(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG147(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG146(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG148(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG147(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG149(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG148(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG150(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG149(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG151(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG150(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG152(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG151(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG153(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG152(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG154(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG153(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG155(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG154(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG156(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG155(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG157(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG156(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG158(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG157(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG159(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG158(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
#define _VA_FRONT_WRAP_ARG160(wrap, ...) _VA_WRAP_INNER(wrap, _VA_ARG0(__VA_ARGS__)), _VA_FRONT_WRAP_ARG159(wrap, _VA_ARGS_SECOND(__VA_ARGS__))
/// @todo support more, but currently supports up to 160

#define _VA_LAST_ARG1(x0, ...) __VA_ARGS__
#define _VA_LAST_ARG2(x0, x1, ...) __VA_ARGS__
#define _VA_LAST_ARG3(x0, x1, x2, ...) __VA_ARGS__
#define _VA_LAST_ARG4(x0, x1, x2, x3, ...) __VA_ARGS__
#define _VA_LAST_ARG5(x0, x1, x2, x3, x4, ...) __VA_ARGS__
#define _VA_LAST_ARG6(x0, x1, x2, x3, x4, x5, ...) __VA_ARGS__
#define _VA_LAST_ARG7(x0, x1, x2, x3, x4, x5, x6, ...) __VA_ARGS__
#define _VA_LAST_ARG8(x0, x1, x2, x3, x4, x5, x6, x7, ...) __VA_ARGS__
#define _VA_LAST_ARG9(x0, x1, x2, x3, x4, x5, x6, x7, x8, ...) __VA_ARGS__
#define _VA_LAST_ARG10(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) __VA_ARGS__
#define _VA_LAST_ARG11(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ...) __VA_ARGS__
#define _VA_LAST_ARG12(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ...) __VA_ARGS__

#define IPC_MSG_SET_ARG1(msg, v1, l1)        \
    do {                                     \
        ipc_msg_set_nth_arg(msg, 0, v1, l1); \
    } while (0)

#define IPC_MSG_SET_ARG2(msg, v1, v2, l1, l2) \
    do {                                      \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);  \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);  \
    } while (0)

#define IPC_MSG_SET_ARG3(msg, v1, v2, v3, l1, l2, l3) \
    do {                                              \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);          \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);          \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);          \
    } while (0)

#define IPC_MSG_SET_ARG4(msg, v1, v2, v3, v4, l1, l2, l3, l4) \
    do {                                                      \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                  \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                  \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                  \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                  \
    } while (0)

#define IPC_MSG_SET_ARG5(msg, v1, v2, v3, v4, v5, l1, l2, l3, l4, l5) \
    do {                                                              \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                          \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                          \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                          \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                          \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                          \
    } while (0)

#define IPC_MSG_SET_ARG6(msg, v1, v2, v3, v4, v5, v6, l1, l2, l3, l4, l5, l6) \
    do {                                                                      \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                  \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                  \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                  \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                  \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                  \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                  \
    } while (0)

#define IPC_MSG_SET_ARG7(msg, v1, v2, v3, v4, v5, v6, v7, l1, l2, l3, l4, l5, l6, l7) \
    do {                                                                              \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                          \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                          \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                          \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                          \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                          \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                          \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                          \
    } while (0)

#define IPC_MSG_SET_ARG8(msg, v1, v2, v3, v4, v5, v6, v7, v8, l1, l2, l3, l4, l5, l6, l7, l8) \
    do {                                                                                      \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                                  \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                                  \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                                  \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                                  \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                                  \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                                  \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                                  \
        ipc_msg_set_nth_arg(msg, 7, v8, l8);                                                  \
    } while (0)

#define IPC_MSG_SET_ARG9(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, l1, l2, l3, l4, l5, l6, l7, l8, l9) \
    do {                                                                                              \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                                          \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                                          \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                                          \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                                          \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                                          \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                                          \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                                          \
        ipc_msg_set_nth_arg(msg, 7, v8, l8);                                                          \
        ipc_msg_set_nth_arg(msg, 8, v9, l9);                                                          \
    } while (0)

#define IPC_MSG_SET_ARG10(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10) \
    do {                                                                                                         \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                                                     \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                                                     \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                                                     \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                                                     \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                                                     \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                                                     \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                                                     \
        ipc_msg_set_nth_arg(msg, 7, v8, l8);                                                                     \
        ipc_msg_set_nth_arg(msg, 8, v9, l9);                                                                     \
        ipc_msg_set_nth_arg(msg, 9, v10, l10);                                                                   \
    } while (0)

#define IPC_MSG_SET_ARG11(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11) \
    do {                                                                                                                   \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                                                               \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                                                               \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                                                               \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                                                               \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                                                               \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                                                               \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                                                               \
        ipc_msg_set_nth_arg(msg, 7, v8, l8);                                                                               \
        ipc_msg_set_nth_arg(msg, 8, v9, l9);                                                                               \
        ipc_msg_set_nth_arg(msg, 9, v10, l10);                                                                             \
        ipc_msg_set_nth_arg(msg, 10, v11, l11);                                                                            \
    } while (0)

#define IPC_MSG_SET_ARG12(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12) \
    do {                                                                                                                             \
        ipc_msg_set_nth_arg(msg, 0, v1, l1);                                                                                         \
        ipc_msg_set_nth_arg(msg, 1, v2, l2);                                                                                         \
        ipc_msg_set_nth_arg(msg, 2, v3, l3);                                                                                         \
        ipc_msg_set_nth_arg(msg, 3, v4, l4);                                                                                         \
        ipc_msg_set_nth_arg(msg, 4, v5, l5);                                                                                         \
        ipc_msg_set_nth_arg(msg, 5, v6, l6);                                                                                         \
        ipc_msg_set_nth_arg(msg, 6, v7, l7);                                                                                         \
        ipc_msg_set_nth_arg(msg, 7, v8, l8);                                                                                         \
        ipc_msg_set_nth_arg(msg, 8, v9, l9);                                                                                         \
        ipc_msg_set_nth_arg(msg, 9, v10, l10);                                                                                       \
        ipc_msg_set_nth_arg(msg, 10, v11, l11);                                                                                      \
        ipc_msg_set_nth_arg(msg, 11, v12, l12);                                                                                      \
    } while (0)

#define IPC_MSG_GET_ARG1(msg, v1, l1)        \
    do {                                     \
        ipc_msg_get_nth_arg(msg, 0, v1, l1); \
    } while (0)

#define IPC_MSG_GET_ARG2(msg, v1, v2, l1, l2) \
    do {                                      \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);  \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);  \
    } while (0)

#define IPC_MSG_GET_ARG3(msg, v1, v2, v3, l1, l2, l3) \
    do {                                              \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);          \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);          \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);          \
    } while (0)

#define IPC_MSG_GET_ARG4(msg, v1, v2, v3, v4, l1, l2, l3, l4) \
    do {                                                      \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                  \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                  \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                  \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                  \
    } while (0)

#define IPC_MSG_GET_ARG5(msg, v1, v2, v3, v4, v5, l1, l2, l3, l4, l5) \
    do {                                                              \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                          \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                          \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                          \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                          \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                          \
    } while (0)

#define IPC_MSG_GET_ARG6(msg, v1, v2, v3, v4, v5, v6, l1, l2, l3, l4, l5, l6) \
    do {                                                                      \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                  \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                  \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                  \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                  \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                  \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                  \
    } while (0)

#define IPC_MSG_GET_ARG7(msg, v1, v2, v3, v4, v5, v6, v7, l1, l2, l3, l4, l5, l6, l7) \
    do {                                                                              \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                          \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                          \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                          \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                          \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                          \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                          \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                          \
    } while (0)

#define IPC_MSG_GET_ARG8(msg, v1, v2, v3, v4, v5, v6, v7, v8, l1, l2, l3, l4, l5, l6, l7, l8) \
    do {                                                                                      \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                                  \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                                  \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                                  \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                                  \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                                  \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                                  \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                                  \
        ipc_msg_get_nth_arg(msg, 7, v8, l8);                                                  \
    } while (0)

#define IPC_MSG_GET_ARG9(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, l1, l2, l3, l4, l5, l6, l7, l8, l9) \
    do {                                                                                              \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                                          \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                                          \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                                          \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                                          \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                                          \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                                          \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                                          \
        ipc_msg_get_nth_arg(msg, 7, v8, l8);                                                          \
        ipc_msg_get_nth_arg(msg, 8, v9, l9);                                                          \
    } while (0)

#define IPC_MSG_GET_ARG10(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10) \
    do {                                                                                                         \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                                                     \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                                                     \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                                                     \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                                                     \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                                                     \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                                                     \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                                                     \
        ipc_msg_get_nth_arg(msg, 7, v8, l8);                                                                     \
        ipc_msg_get_nth_arg(msg, 8, v9, l9);                                                                     \
        ipc_msg_get_nth_arg(msg, 9, v10, l10);                                                                   \
    } while (0)

#define IPC_MSG_GET_ARG11(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11) \
    do {                                                                                                                   \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                                                               \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                                                               \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                                                               \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                                                               \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                                                               \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                                                               \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                                                               \
        ipc_msg_get_nth_arg(msg, 7, v8, l8);                                                                               \
        ipc_msg_get_nth_arg(msg, 8, v9, l9);                                                                               \
        ipc_msg_get_nth_arg(msg, 9, v10, l10);                                                                             \
        ipc_msg_get_nth_arg(msg, 10, v11, l11);                                                                            \
    } while (0)

#define IPC_MSG_GET_ARG12(msg, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12) \
    do {                                                                                                                             \
        ipc_msg_get_nth_arg(msg, 0, v1, l1);                                                                                         \
        ipc_msg_get_nth_arg(msg, 1, v2, l2);                                                                                         \
        ipc_msg_get_nth_arg(msg, 2, v3, l3);                                                                                         \
        ipc_msg_get_nth_arg(msg, 3, v4, l4);                                                                                         \
        ipc_msg_get_nth_arg(msg, 4, v5, l5);                                                                                         \
        ipc_msg_get_nth_arg(msg, 5, v6, l6);                                                                                         \
        ipc_msg_get_nth_arg(msg, 6, v7, l7);                                                                                         \
        ipc_msg_get_nth_arg(msg, 7, v8, l8);                                                                                         \
        ipc_msg_get_nth_arg(msg, 8, v9, l9);                                                                                         \
        ipc_msg_get_nth_arg(msg, 9, v10, l10);                                                                                       \
        ipc_msg_get_nth_arg(msg, 10, v11, l11);                                                                                      \
        ipc_msg_get_nth_arg(msg, 11, v12, l12);                                                                                      \
    } while (0)

#define IPC_DO_SERVE1(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0])
#define IPC_DO_SERVE2(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1])
#define IPC_DO_SERVE3(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2])
#define IPC_DO_SERVE4(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3])
#define IPC_DO_SERVE5(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4])
#define IPC_DO_SERVE6(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5])
#define IPC_DO_SERVE7(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6])
#define IPC_DO_SERVE8(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7])
#define IPC_DO_SERVE9(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8])
#define IPC_DO_SERVE10(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9])
#define IPC_DO_SERVE11(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10])
#define IPC_DO_SERVE12(ipc_name) IPC_DO_SERVE_FUNC(ipc_name)(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11])
