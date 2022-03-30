#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

// 基本信号处理函数
void hand(int n)
{
    printf("基本信号处理函数\n");
}

// 高级信号处理函数
void handler(int n,siginfo_t* siginfo,void* arg)
{
    printf("高级信号处理函数\n");
    printf("n:%d msg:%d\n",n,siginfo->si_int);
}

int main(void)
{
    struct sigaction act = {0};
    struct sigaction old_act = {0};
    act.sa_handler = hand;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    // 高级信号处理
    sigaction(2,&act,&old_act);
    printf("pid:%d\n",getpid());

    int n = 0;
    while (true) {
        printf("n:%d\n",n++);
        sleep(1);
    }
    return 0;
}