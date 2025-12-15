#include "hal_base.h"
#include "rpmsg_lite.h"
#include "rpmsg_ns.h"
#include "board.h"
#include <device.h>
#include <xizi.h>
#include <shell.h>
#include "string.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RPMSG_LITE_SHMEM_BASE         (VDEV0_VRING_BASE)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-tty"
#define RPMSG_LITE_MASTER_IS_LINUX

#define APP_TASK_STACK_SIZE (256U)
#define TC_LOCAL_EPT_ADDR (30)
// #define TC_REMOTE_EPT_ADDR (40)

struct rpmsg_lite_endpoint *volatile my_ept = NULL;
struct rpmsg_lite_instance *volatile my_rpmsg = NULL;
rpmsg_ns_handle ns_handle = NULL;

int rc_finish = 0;
int remote_ept_addr = 0;
static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
    KPrintf("get new name: %s\n", new_ept_name);
    remote_ept_addr = new_ept;
}

int32_t ept_cb(void *data, uint32_t len, uint32_t src, void *priv) {
    int32_t result = 0;
    remote_ept_addr = src;
    KPrintf("receive msg from %d\n", src);
    // if (src != TC_REMOTE_EPT_ADDR) KPrintf("receive error!, src is %d\n", src);
    KPrintf("receive msg: ");
    for(int i=0; i<len; i++) {
        KPrintf("%c", ((char *)data)[i]);
    }
    KPrintf("\n");
    
    // Send acknowledgment message
    if (my_rpmsg != NULL && my_ept != NULL && remote_ept_addr != 0) {
        char ack_msg[] = "Message received";
        rpmsg_lite_send(my_rpmsg, my_ept, remote_ept_addr, ack_msg, strlen(ack_msg)+1, RL_DONT_BLOCK);
        KPrintf("send ack msg: %s\n", ack_msg);
    }
    return 0;
}

void RPMsgTask_Entry(void *param)
{
    KPrintf("M0 rpmsg task begins!\n");
    // link_id is not used in code
    my_rpmsg = rpmsg_lite_remote_init((void *)0x00100000, 4, RL_NO_FLAGS);
    while (0 == rpmsg_lite_is_link_up(my_rpmsg)) {   }
    KPrintf("Link is up!\n");
    KPrintf("nameservice bind begin\n");

    ns_handle = rpmsg_ns_bind(my_rpmsg, app_nameservice_isr_cb, ((void *)0));
    // SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    HAL_DelayUs(1000000U);
    KPrintf("nameservice ept created, begin to create local ept\n");
    my_ept = rpmsg_lite_create_ept(my_rpmsg, TC_LOCAL_EPT_ADDR, ept_cb, NULL);
    // announce a new endpoint to communicate with remote, the channel name is RPMSG_LITE_NS_ANNOUNCE_STRING
    rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, (uint32_t)RL_NS_CREATE);
    KPrintf("rpmsg_ns_announce succeeds\n");
    KPrintf("RPMsg is running, waiting for messages...\n");

    // Keep running and waiting for messages
    // Messages will be handled by ept_cb callback function
    while(1) {
        // Sleep to avoid busy waiting, messages are handled in callback
        env_sleep_msec(100);
    }
    
    // This code should never be reached, but kept for safety
    KPrintf("M0 Finish!\n");
    rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
    rpmsg_ns_unbind(my_rpmsg, ns_handle);
    rpmsg_lite_deinit(my_rpmsg);
}

void CreateRPMsgTask(void)
{
    int32 rpmsg_task_id = KTaskCreate("rpmsg_task",RPMsgTask_Entry,NULL,2048,SHELL_TASK_PRIORITY + 1);

    if (rpmsg_task_id < 0)
    {
        KPrintf("CreateRPMsgTask: create rpmsg task fail!\n");
        return ;
    }
    KPrintf("CreateRPMsgTask: create rpmsg task success!\n");

    int32 start_up_result = StartupKTask(rpmsg_task_id);

    if (start_up_result != 0)
    {
        KPrintf("CreateRPMsgTask: startup rpmsg task fail\n");
    }
    KPrintf("CreateRPMsgTask: startup rpmsg task success\n");
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(0)|SHELL_CMD_DISABLE_RETURN, CreateRPMsgTask, CreateRPMsgTask, create rpmsg listener task);
