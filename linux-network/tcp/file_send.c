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

int clnt_sock;

int main(int argc,char* argv[]) 
{
    clnt_sock = socket(AF_INET,SOCK_STREAM,0);
    if (clnt_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ADDR);
    serv_addr.sin_port = htons(PORT);

    int ret = connect(clnt_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("connect error:%m\n");
        exit(-1);
    }
    printf("connect success\n");

    char name[256] = {0};
    printf("input the file name: ");
    scanf("%s",name);
    // 打开文件
    int fd = open(name,O_RDONLY);
    if (fd == -1) {
        printf("open error:%m\n");
        exit(-1);
    }
    printf("open %s successfully\n",name);

    struct stat st = {0};
    stat(name,&st);
    printf("File size: %d\n",st.st_size);

    send(clnt_sock,name,strlen(name)+1,0);
    send(clnt_sock,(char*)&st.st_size,4,0);

    char buff[1024] = {0};
    while(true) {
        ret = read(fd,buff,1024);
        if (ret > 0) {
            send(clnt_sock,buff,ret,0);
        }
        else
            break;
    }
    printf("send ok\n");
    sleep(1);
    close(clnt_sock);
    
    return 0;
}