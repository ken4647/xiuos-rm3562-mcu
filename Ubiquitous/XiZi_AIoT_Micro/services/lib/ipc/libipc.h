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
 * @file libipc.h
 * @brief libipc header
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2023.08.25
 */

/*************************************************
File name: libipc.h
Description: libipc header
Others:
History:
1. Date: 2023-08-28
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ipcargs.h"
#include "session.h"

#define NR_MAX_SESSION 32
#define IPC_MSG_MAGIC 0xABCDDCBA

typedef struct {
    union {
        uint64_t header;
        struct {
            uint64_t valid : 1; // for server to peek new msg
            uint64_t done : 1; // for client to check request done
            uint64_t init : 1; // for client to check request done
            uint64_t handling : 1;
            uint64_t nr_args : 4;
            uint64_t opcode : 8;
            uint64_t len : 16;
            uint64_t magic : 32;
        };
    };
    int32_t ret_val;
} __attribute__((packed)) ipc_msg_header;

struct IpcArgInfo {
    uint16_t offset;
    uint16_t len;
    union {
        uint16_t attr;
        struct {
            uint16_t null_ptr : 1;
            uint16_t reserved : 15;
        };
    };
} __attribute__((packed));

/* [header, ipc_arg_buffer_len[], ipc_arg_buffer[]] */
struct IpcMsg {
    ipc_msg_header header;
    uintptr_t buf[];
} __attribute__((packed));
enum {
    IPC_ARG_INFO_BASE_OFFSET = sizeof(ipc_msg_header),
};

typedef int (*IpcInterface)(struct IpcMsg* msg);

struct IpcNode {
    char* name;
    IpcInterface interfaces[UINT8_MAX];
} __attribute__((packed));

#define IPC_SERVER_LOOP(ipc_node_name) rpc_server_loop_##rpc_node_name
#define IPC_SERVICES(ipc_node_name, ...)         \
    typedef enum {                               \
        ipc_node_name##_IPC_OP_CODE_DEFAULT = 0, \
        __VA_ARGS__,                             \
        ipc_node_name##_IPC_OP_CODE_END,         \
    } IpcOpCode_##ipc_node_name;

#define _IPC_INSERT_SERVE(interface) [interface] = IPC_SERVE(interface)
#define IPC_SERVER_REGISTER_INTERFACES(ipc_node_name, num_interfaces, ...)      \
    static struct IpcNode ipc_node_name = {                                     \
        .interfaces = {                                                         \
            _VA_FRONT_WRAP_ARG##num_interfaces(_IPC_INSERT_SERVE, __VA_ARGS__), \
        },                                                                      \
    }

#define IPCSESSION_MSG(session) ((struct IpcMsg*)((char*)((session)->buf) + (session)->head))
#define IPCMSG_ARG_INFO_BASE(msg) ((struct IpcArgInfo*)((char*)msg + sizeof(ipc_msg_header)))
#define IPCMSG_ARG_INFO(msg, nth) ((struct IpcArgInfo*)((char*)IPCMSG_ARG_INFO_BASE(msg) + (nth * sizeof(struct IpcArgInfo))))

/// @brief get buffer of nth arg in ipc_msg
/// @param msg
/// @param arg_num start with 0 for first arg
/// @return
__attribute__((__always_inline__)) static inline void* ipc_msg_get_nth_arg_buf(struct IpcMsg* msg, int arg_num)
{
    if (IPCMSG_ARG_INFO(msg, arg_num)->null_ptr == 1) {
        return NULL;
    }

    return (void*)((char*)msg + IPCMSG_ARG_INFO(msg, arg_num)->offset);
}

/// @brief get return val of ipc call after done, used by client
/// @param msg
/// @param ret_val
__attribute__((__always_inline__)) static inline void ipc_msg_get_return(struct IpcMsg* msg, int32_t* ret_val)
{
    assert(msg->header.done == 1);
    memcpy(ret_val, &msg->header.ret_val, sizeof(msg->header.ret_val));
    return;
}

/// @brief set return val of ipc call after serve, used by server
/// @param msg
/// @param ret_val
__attribute__((__always_inline__)) static inline void ipc_msg_set_return(struct IpcMsg* msg, int32_t* ret_val)
{
    assert(msg->header.valid == 1);
    memcpy(&msg->header.ret_val, ret_val, sizeof(msg->header.ret_val));
    return;
}

/// @brief set opcode to msg
/// @param msg
/// @param opcode
/// @return
__attribute__((__always_inline__)) static inline bool ipc_msg_set_opcode(struct IpcMsg* msg, int opcode)
{
    if (opcode < 0 || opcode > UINT8_MAX) {
        return false;
    }
    msg->header.opcode = opcode;
    return true;
}

/// @brief get opcode from msg
/// @param msg
/// @return opcode, -1 if opcode is invalid
__attribute__((__always_inline__)) static inline int ipc_msg_get_opcode(struct IpcMsg* msg)
{
    assert(msg->header.valid == 1);
    int opcode = msg->header.opcode;
    if (opcode < 0 || opcode > UINT8_MAX) {
        return -1;
    }
    return opcode;
}

/// @brief delete first msg in session
/// @param session
/// @return
__attribute__((__always_inline__)) static inline bool ipc_session_forward(struct Session* session)
{
    struct IpcMsg* msg = IPCSESSION_MSG(session);
    if (msg->header.init != 1) {
        return false;
    }
    return session_free_buf(session, msg->header.len);
}

struct IpcMsg* new_ipc_msg(struct Session* session, const int argc, const int* arg_size);
bool ipc_msg_set_nth_arg(struct IpcMsg* msg, const int arg_num, const void* const data, const int len);
bool ipc_msg_get_nth_arg(struct IpcMsg* msg, const int arg_num, void* data, const int len);
void ipc_msg_send_wait(struct Session* session, struct IpcMsg* msg);
void ipc_msg_send_nowait(struct IpcMsg* msg);
int ipc_session_wait(struct Session* session);

void ipc_server_loop(struct IpcNode* ipc_node);

#define IPC_CREATE_MSG_FUNC(ipc_name) ipc_msg_##ipc_name
#define IPC_MSG_ARGS_COPY_SET_FUNC(ipc_name) ipc_msg_args_copy_set_##ipc_name
#define IPC_MSG_ARGS_COPY_GET_FUNC(ipc_name) ipc_msg_args_copy_get_##ipc_name
#define IPC_CALL(ipc_name) ipc_call_copy_args_##ipc_name

#define IPC_SERVE(ipc_name) ipc_serve_##ipc_name
#define IPC_THREAD_SERVE(ipc_name) ipc_thread_serve_##ipc_name
#define IPC_DO_SERVE_FUNC(ipc_name) ipc_do_serve_##ipc_name

/// when defining a ipc server:
///     1. requires a IPC_SERVICES(server_name, interface_name, ...) to announce the name and interfaces that the server will support
///     2. implement IPC_DO_SERVE_FUNC(interface_name) for each interface
///     3. use IPC_SERVER_INTERFACE(interface_name, argc) to generate necessary helper functions
///     4. use IPC_SERVER_REGISTER_INTERFACES(server_name, nr_interfaces, interface_names, ...) to bind interfaces to server after implementations
///     5. use ipc_server_loop in main()
///     Refer to simple_service.h, simple_service.c and simple_server.c for example

#define IPC_INTERFACE(ipc_name, argc, ...)                                                                                                     \
    __always_inline static inline struct IpcMsg* IPC_CREATE_MSG_FUNC(ipc_name)(struct Session * session, _VA_FRONT_PTR_ARG##argc(__VA_ARGS__)) \
    {                                                                                                                                          \
        int argv[argc] = { _VA_LAST_ARG##argc(__VA_ARGS__) };                                                                                  \
        return new_ipc_msg(session, argc, argv);                                                                                               \
    }                                                                                                                                          \
                                                                                                                                               \
    __always_inline static inline bool IPC_MSG_ARGS_COPY_SET_FUNC(ipc_name)(struct IpcMsg * msg, _VA_FRONT_PTR_ARG##argc(__VA_ARGS__))         \
    {                                                                                                                                          \
        IPC_MSG_SET_ARG##argc(msg, __VA_ARGS__);                                                                                               \
        return true;                                                                                                                           \
    }                                                                                                                                          \
                                                                                                                                               \
    __always_inline static inline bool IPC_MSG_ARGS_COPY_GET_FUNC(ipc_name)(struct IpcMsg * msg, _VA_FRONT_PTR_ARG##argc(__VA_ARGS__))         \
    {                                                                                                                                          \
        IPC_MSG_GET_ARG##argc(msg, __VA_ARGS__);                                                                                               \
        return true;                                                                                                                           \
    }                                                                                                                                          \
                                                                                                                                               \
    static int IPC_CALL(ipc_name)(struct Session * session, _VA_FRONT_PTR_ARG##argc(__VA_ARGS__))                                              \
    {                                                                                                                                          \
        if (session == NULL) {                                                                                                                 \
            return -1;                                                                                                                         \
        }                                                                                                                                      \
        struct IpcMsg* msg = IPC_CREATE_MSG_FUNC(ipc_name)(session, _VA_FRONT_ARG##argc(__VA_ARGS__));                                         \
        int ret = IPC_MSG_ARGS_COPY_SET_FUNC(ipc_name)(msg, _VA_FRONT_ARG##argc(__VA_ARGS__));                                                 \
        ret = ipc_msg_set_opcode(msg, ipc_name);                                                                                               \
        ipc_msg_send_wait(session, msg);                                                                                                       \
        ret = IPC_MSG_ARGS_COPY_GET_FUNC(ipc_name)(msg, _VA_FRONT_ARG##argc(__VA_ARGS__));                                                     \
        int32_t res = 0;                                                                                                                       \
        ipc_msg_get_return(msg, &res);                                                                                                         \
        ipc_session_forward(session);                                                                                                          \
        return res;                                                                                                                            \
    }

#define IPC_SERVER_INTERFACE(ipc_name, argc)            \
    static int IPC_SERVE(ipc_name)(struct IpcMsg * msg) \
    {                                                   \
        void* argv[argc];                               \
        for (int i = 0; i < argc; i++) {                \
            argv[i] = ipc_msg_get_nth_arg_buf(msg, i);  \
        }                                               \
        int32_t _ret = IPC_DO_SERVE##argc(ipc_name);    \
        ipc_msg_set_return(msg, &_ret);                 \
        msg->header.done = 1;                           \
        return 0;                                       \
    }

void _ipc_addr_to_buf(uintptr_t addr, char buf[17]);
uintptr_t _ipc_buf_to_addr(char* buf);

#define IPC_SERVER_THREAD_INTERFACE(ipc_name, argc)                         \
    static int IPC_THREAD_SERVE(ipc_name)(int ipc_argc, char** ipc_argv)    \
    {                                                                       \
        if (ipc_argc != 2) {                                                \
            printf("[%s] Error server thread creation.\n", __func__);       \
            exit(1);                                                        \
            return -1;                                                      \
        }                                                                   \
        struct IpcMsg* msg = (struct IpcMsg*)_ipc_buf_to_addr(ipc_argv[1]); \
        void* argv[argc];                                                   \
        for (int i = 0; i < argc; i++) {                                    \
            argv[i] = ipc_msg_get_nth_arg_buf(msg, i);                      \
        }                                                                   \
        int32_t _ret = IPC_DO_SERVE##argc(ipc_name);                        \
        ipc_msg_set_return(msg, &_ret);                                     \
        msg->header.done = 1;                                               \
        exit(0);                                                            \
        return 0;                                                           \
    }                                                                       \
    static int IPC_SERVE(ipc_name)(struct IpcMsg * msg)                     \
    {                                                                       \
        char addr_buf[17];                                                  \
        _ipc_addr_to_buf((uintptr_t)msg, addr_buf);                         \
        char* param[] = { #ipc_name, addr_buf, NULL };                      \
        msg->header.handling = 1;                                           \
        int tid = thread(IPC_THREAD_SERVE(ipc_name), #ipc_name, param);     \
        if (tid <= 0) {                                                     \
            msg->header.handling = 0;                                       \
        }                                                                   \
        return 0;                                                           \
    }

int cur_session_id(void);