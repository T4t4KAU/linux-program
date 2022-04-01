#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    // 创建管道文件
    int ret = mkfifo("test.pipe",0);
    if (-1 == ret) {
        printf("创建管道文件失败:%m\n");
        exit(-1);
    }
    printf("创建管道文件成功\n");
    // 打开管道文件
    int fd = open("test.pipe",O_WRONLY);
    if (-1 == fd) {
        printf("打开管道文件失败:%m\n");
        unlink("test.pipe");
        exit(-1);
    }
    printf("打开管道文件成功\n");
    // 循环写入
    int n = 0;
    char buff[1024] = {0};
    while(true) {
        sprintf(buff,"Hello Linux %d",n++);
        write(fd,buff,strlen(buff));
        sleep(1);
    }
    close(fd);
    unlink("test.pipe");
    return 0;
}