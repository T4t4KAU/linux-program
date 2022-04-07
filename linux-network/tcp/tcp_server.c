#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int serv_sock,clnt_sock;

void hand() {
    // 断开连接
    close(clnt_sock);
    close(serv_sock);
    printf("bye\n");
    exit(0);
}

int main(int argc,char* argv[])
{
    if (argc != 3) {
        printf("input error\n");
        exit(-1);
    }

    signal(2,hand);

    // 创建socket
    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if (serv_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    // 创建套接字地址协议簇
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // IP地址 字符串->点分
    serv_addr.sin_port = htons(atoi(argv[2])); // 转字节序

    // 绑定
    int ret = bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        close(serv_sock);
        exit(-2);
    }
    printf("bind success\n");

    // 监听
    ret = listen(serv_sock,10);
    if (-1 == ret) {
        printf("listen error:%m\n");
        close(serv_sock);
        exit(-2);
    }
    printf("listen success\n");

    struct sockaddr_in clnt_addr = {0};
    int len = sizeof(clnt_addr);
    // 接收连接
    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&len);
    if (clnt_sock == -1) {
        printf("accept error:%m");
        close(serv_sock);
        exit(-2);
    }
    printf("accept success\n");
    printf("%s is connecting\n",inet_ntoa(clnt_addr.sin_addr));

    // 通信
    char buff[256] = {0};
    while (true) {
        ret = recv(clnt_sock,buff,256,0);
        if (ret > 0) {
            buff[ret] = 0;
            printf(">> %s\n",buff);
        }
    }
    return 0;
}