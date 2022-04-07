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

void hand()
{
    close(clnt_sock);
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
    clnt_sock = socket(AF_INET,SOCK_DGRAM,0);
    if (clnt_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    printf("ip:%s port:%s\n",argv[1],argv[2]);

    int len = sizeof(serv_addr);
    char buff[256] = {0};
    char temp[256] = {0};
    while(true) {
        printf("%s << ",argv[1]);
        scanf("%s",buff);
        sendto(clnt_sock,buff,sizeof(buff),0,
            (struct sockaddr*)&serv_addr,sizeof(serv_addr));
        int ret = recvfrom(clnt_sock,temp,255,0,
                (struct sockaddr*)&serv_addr,&len);
        
        if (ret > 0) {
            temp[ret] = 0;
            printf("%s >> %s\n",argv[1],temp);
        }
    }
    
    return 0;
}