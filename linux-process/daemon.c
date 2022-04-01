#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>

int main(void)
{
    // 重设文件权限
    umask(0);
    // 创建子进程 结束父进程
    int ret = fork();
    if (ret < 0) {
        printf("create process failed: %m\n");
        exit(-1);
    }
    if (ret > 0) {
        printf("parent process end\n");
        exit(0);
    }
    if (0 == ret) {
        printf("pid: %d\n",getpid());
        // 创建新会话
        setsid();
        // 防止子进程成为僵尸进程 忽略SIGCHILD、SIGUP信号
        signal(SIGCHLD,SIG_IGN);
        signal(SIGHUP,SIG_IGN);
        // 改变当前工作目录
        chdir("/");
        // 重定向文件描述符号 open、dup函数
        int fd = open("/dev/null",O_RDWR);
        dup2(fd,0);
        dup2(fd,1);
    }
    //模拟守护进程工作
    while (true) {
        sleep(1);
    }
    return 0;
}