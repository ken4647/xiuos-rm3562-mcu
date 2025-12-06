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
* @file board.h
* @brief define stm32f407zgt6-board init configure and start-up function
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2021-04-25
*/

/*************************************************
File name: board.h
Description: define stm32f407zgt6-board board init function and struct
Others: 
History: 
1. Date: 2021-04-25
Author: AIIT XUOS Lab
Modification: 
1. define stm32f407zgt6-board InitBoardHardware
2. define stm32f407zgt6-board data and bss struct
*************************************************/

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

extern  void *__bss_end;
extern  void *_heap_end;
extern unsigned int g_service_table_start;
extern unsigned int g_service_table_end;


#define MEM_OFFSET  0x0000 
#define MEMORY_START_ADDRESS       ( ( ((unsigned long)(&__bss_end)) > MEM_OFFSET)? (unsigned long)(&__bss_end):(MEM_OFFSET)  )
#define MEMORY_END_ADDRESS         ( &_heap_end )

void InitBoardHardware(void);

#endif