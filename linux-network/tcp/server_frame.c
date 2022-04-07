#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 9527
#define BACKLOG 2

extern void sig_proc(int signo);
extern void sig_pipe(int signo);
void process_conn_server(int clnt_sock);

int main(int argc,char* argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    pid_t pid;
    signal(SIGINT,sig_proc);
    signal(SIGPIPE,sig_pipe);

    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if (serv_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    int ret = bind(serv_sock,(struct sockaddr*)serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        exit(-1)
    };
    printf("bind success\n");

    ret = listen(serv_sock,BACKLOG);
    if (ret == -1) {
        printf("listen error:%m\n");
        exit(-1);
    }
    printf("listen success\n");

    while (true) {
        int len = sizeof(struct sockaddr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)clnt_addr,&len);
        if (clnt_sock == -1) {
            printf("accept error:%m\n");
            continue;
        }

        pid = fork();
        if (pid == 0) {
            close(serv_sock);
            process_conn_server(clnt_sock);
        }
        else
            close(clnt_sock);
    }

    return 0;
}

void process_conn_server(int clnt_sock)
{
    ssize_t size = 0;
    char buff[1024] = {0};

    while (true) {
        size = recv(clnt_sock,buff,1024,0);
        if (size == 0)
            return;
        sprintf(buff,"%d bytes altogether\n",size);
        send(clnt_sock,strlen(buff)+1,0);
    }
}