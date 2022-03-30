#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void hand(int n) {
    printf("响应信号\n");
}

int main(void)
{
    sigset_t set,old_set;
    int ret;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    // 前5秒不设置信号屏蔽
    signal(SIGINT,hand);
    sleep(5);
    // 设置信号屏蔽20秒
    if (1 == sigismember(&set,SIGINT)) {
        printf("设置型号屏蔽\n");
        ret = sigprocmask(SIG_BLOCK,&set,&old_set);
        if (0 == ret) printf("设置信号屏蔽成功\n");
        else printf("设置信号屏蔽失败\n");
    }
    sleep(20);
    
    if (1 == sigismember(&set,SIGINT)) {
        printf("解除信号屏蔽\n");
        ret = sigprocmask(SIG_UNBLOCK,&set,&old_set);
        if (0 == ret) printf("设置信号屏蔽成功\n");
        else printf("设置信号屏蔽失败\n");
    }    

    while(1);
    return 0;
}