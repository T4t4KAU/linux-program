#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define ADDR "127.0.0.1"
#define PORT 9527

int serv_sock,clnt_sock;

int main(void) 
{
    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if (serv_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ADDR);
    serv_addr.sin_port = htons(PORT);

    int ret = bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        exit(-1);
    }
    printf("bind success\n");

    ret = listen(serv_sock,10);
    if (ret == -1) {
        printf("listen error:%m\n");
        exit(-1);
    }   
    printf("listen success\n");

    struct sockaddr_in clnt_addr = {0};
    int len = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&len);
    if (clnt_sock == -1) {
        printf("accept error:%m\n");
        exit(-1);
    }
    printf("accept success\n");
    printf("%s is connecting\n",inet_ntoa(clnt_addr.sin_addr));

    char buff[1024] = {0}; // 缓冲区 存放文件内容
    char name[256] = {0};  // 文件名
    int size = 0;
    // 先接收文件名和文件大小
    ret = recv(clnt_sock,name,255,0);
    if (ret > 0) {
        name[ret] = 0;
        printf("File name: %s\n",name);
    }
    ret = recv(clnt_sock,(char*)&size,4,0);
    if (ret == 4) {
        printf("File size: %d\n",size);
    }

    int count = 0; // 已经接收的大小
    int fd = open("./recv.txt",O_CREAT|O_WRONLY,0666); // 打开文件
    while (true) {
        ret = recv(clnt_sock,buff,1024,0);
        if (ret > 0) {
            write(fd,buff,ret);
            count += ret;
            printf("recv size: %d\n",count);
            if (count >= size) 
                break;
        }
        else
            break;
    }

    close(fd);
    sleep(1);
    printf("receive ok\n");
    close(clnt_sock);
    close(serv_sock);
    return 0;
}