#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int serv_sock;

void hand()
{
    close(serv_sock);
    printf("bye\n");
    exit(0);
}

int main(int argc,char* argv[])
{
    if (argc != 3) {
        printf("input error\n");
        exit(0);
    }
    signal(2,hand);
    printf("ip:%s port:%s\n",argv[1],argv[2]);
    
    serv_sock = socket(AF_INET,SOCK_DGRAM,0);
    if (-1 == serv_sock) {
        printf("socket error:%m\n");
        exit(-1);
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    int ret = bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        exit(-2);
    }

    char buff[256] = {0};
    struct sockaddr_in clnt_addr = {0};
    int len = sizeof(clnt_addr);
    while(true) {
        ret = recvfrom(serv_sock,buff,255,0,
            (struct sockaddr*)&clnt_addr,&len);
        if (ret > 0) {
            buff[ret] = 0;
            printf("%s >> %s\n",inet_ntoa(clnt_addr.sin_addr),buff);
        }
        sendto(serv_sock,"Hello Linux",sizeof("Hello Linux"),
            0,(struct sockaddr*)&clnt_addr,sizeof(clnt_addr));
    }

    return 0;
}