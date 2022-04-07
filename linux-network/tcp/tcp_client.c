#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int clnt_sock;

int main(int argc,char* argv[])
{
    if (argc != 3) {
        printf("input error\n");
        exit(-1);
    }

    clnt_sock = socket(AF_INET,SOCK_STREAM,0);
    if (clnt_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 连接服务器
    int ret = connect(clnt_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("connect error:%m\n");
        exit(-1);
    }
    printf("connect success\n");

    char buff[256] = {0};
    while (true) {
        printf("<< ");
        scanf("%s",buff);
        ret = send(clnt_sock,buff,strlen(buff),0); 
    }

    return 0;
}