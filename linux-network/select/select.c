#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdbool.h>

int main(void)
{
    fd_set fds; // 描述符号集合
    FD_ZERO(&fds); // 清空
    FD_SET(0,&fds); // 设值

    int ret;
    char buff[1024] = {0};
    while (true) {
        ret = select(1,&fds,NULL,NULL,NULL);
        if (ret > 0) {
            printf("return: %d\n",ret);
            scanf("%s",buff);
            printf("recv: %s\n",buff);
        }
        else {
            printf("none\n");
        }
    }
    return 0;
}