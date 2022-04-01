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
    // 打开管道文件
    int fd = open("test.pipe",O_RDONLY);
    if (-1 == fd) {
        printf("打开管道文件失败:%m\n");
        unlink("test.pipe");
        exit(-1);
    }
    printf("打开管道文件成功\n");
    // 循环读取
    while (true) {
        char buff[1024] = {0};
        int ret = read(fd,buff,1023);
        if (ret > 0) {
            buff[ret] = 0;
            printf(">>> %s\n",buff);
        }
    }
    close(fd);
    unlink("test.pipe");
    return 0;
}