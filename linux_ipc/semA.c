#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <stdbool.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

// 创建信号量
int create_sem(key_t key,int value)
{
    union semun sem;
    sem.val = value;
    int semid = semget(key,0,IPC_CREAT|0666);
    if (-1 == semid) {
        printf("semget error:%m\n");
        return -1;
    }
    semctl(semid,0,SETVAL,sem);
    return semid;
}

// 设置信号量值
void set_sem_value(int semid,int value)
{
    union semun sem;
    sem.val = value;
    semctl(semid,0,SETVAL,sem);
}

// 获取信号量值
int get_sem_value(int semid)
{
    union semun sem;
    return semctl(semid,0,GETVAL,sem);
}

// 销毁信号量
void destory_sem(int semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid,0,IPC_RMID,sem);
}

// 增加信号量
int add_sem_value(int semid)
{
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = IPC_NOWAIT;
    return (semop(semid,&sops,1));
}

// 减少信号量
int sub_sem_value(int semid)
{
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = IPC_NOWAIT;
    return (semop(semid,&sops,1));
}

int main(void)
{
    key_t key = ftok(".",'m');
    if (key == -1) {
        printf("ftok error:%m\n");
        exit(-1);
    }
    printf("ftok success\n");
    int semid = create_sem(key,100);
    for (int i=0;i<=3;i++)
    {
        add_sem_value(semid);
        sub_sem_value(semid);
    }
    int value = get_sem_value(semid);
    printf("value: %d\n",value);
    destory_sem(semid);
    return 0;
}