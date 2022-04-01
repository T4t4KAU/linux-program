#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

// 使用匿名管道
int main(void)
{
    // 创建管道描述符号
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {
        printf("创建管道失败\n");
        exit(-1);
    }
    printf("创建管道成功\n");
    // 父子进程之间使用管道
    if (fork()) {
        char buff[1024] = {0};
        while(true) {
            scanf("%s",buff);
            write(fd[1],buff,strlen(buff));
        }
    }
    else {
        char temp[1024] = {0};
        while(true) {
            ret = read(fd[0],temp,1023);
            if (ret > 0) {
                temp[ret] = 0;
                printf(">> %s\n",temp);
            }
        }
    }
    close(fd[0]);
    close(fd[1]);
    return 0;
}