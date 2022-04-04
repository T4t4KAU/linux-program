#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define MAX_LEN 64

struct msg_mbuf
{
    int type;
    char text[64];
};

int main(void)
{
    key_t key = ftok(".",'z');
    if (key == -1) {
        printf("ftok error:%m\n");
        exit(-1);
    }
    int msgid = msgget(key,0x666|IPC_CREAT|O_WRONLY);
    if (msgid == -1) {
        printf("msgget error:%m\n");
        exit(-1);
    }
    
    struct msg_mbuf data;
    strcpy(data.text,"hello message");
    int ret  = msgsnd(msgid,(void*)&data,sizeof(data),0);
    if (ret == -1) {
        printf("msgsnd error:%m\n");
        exit(-1);
    }
    sleep(50);
    return 0;
}