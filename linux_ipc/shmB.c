#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

int* p = NULL;
void hand() {
    // 卸载共享内存
    shmdt(p);
    printf("bye\n");
    exit(0);
}

int main(void)
{
    signal(2,hand);
    // 创建key
    key_t key = ftok(".",'m');
    if (-1 == key) {
        printf("ftok error:%m\n");
        exit(-1);
    }
    printf("ftok success\n");
    // 获取共享内存
    int shmid = shmget(key,4096,IPC_CREAT);
    if (-1 == shmid) {
        printf("shmget error:%m\n");
        exit(-1);
    }
    printf("shmget success\n");
    // 挂载共享内存
    p = (int*)shmat(shmid,NULL,0);
    if ((int*)-1 == p) {
        printf("shmat error:%m\n");
        exit(-1);
    }
    printf("shmat success\n");
    // 使用共享内存
    while(true) {
        printf("%d\n",*p);
        sleep(1);
    }
    return 0;
}