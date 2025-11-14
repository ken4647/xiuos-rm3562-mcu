/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */
#include <stdbool.h>

#include "libserial.h"
#include "session.h"
#include "shell.h"
#include "usyscall.h"

Shell shell;
char shellBuffer[512];
struct Session session_fs;

signed short userShellWrite(char* data, unsigned short len)
{
    unsigned short length = len;
    while (length--) {
        printf("%c", *data++);
    }
    return len;
}

signed short userShellRead(char* data, unsigned short len)
{
    unsigned short length = len;
    static char cur_read;
    while (length--) {
        cur_read = getc();
        if (cur_read == 0xff) {
            yield(SYS_TASK_YIELD_NO_REASON);
        }
        // *data++ = getc();
        *data++ = cur_read;
    }
    return len;
}

int main(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;

    shellInit(&shell, shellBuffer, 512);

    while (connect_session(&session_fs, "MemFS", 0x10000) < 0)
        ;
    if (!session_fs.buf) {
        printf("session connect faield\n");
        return -1;
    }
    shellTask(&shell);

    free_session(&session_fs);

    exit(0);
    return 0;
}
