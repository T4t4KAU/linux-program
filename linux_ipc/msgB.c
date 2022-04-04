#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <fcntl.h>

struct msg_mbuf
{
    int type;
    char text[64];
};

int main(void)
{
    key_t key = ftok(".",'z');
    if (key == -1) {
        printf("ftok error:%m");
        exit(-1);
    }
    int msgid = msgget(key,O_RDONLY);
    if (msgid == -1) {
        printf("msgget error:%m\n");
        exit(-1);
    }

    struct msg_mbuf data;
    int ret = msgrcv(msgid,&data,sizeof(data),0,0);
    if (ret == -1) {
        printf("msgrcv error:%m\n");
        exit(-1);
    }
    data.text[ret] = 0;
    printf("%s\n",data.text);
    return 0;
}