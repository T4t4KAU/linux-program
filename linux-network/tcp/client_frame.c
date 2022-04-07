#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 9527

extern void sig_proc(int signo);
extern void sig_pipe(int signo);
static int clnt_sock;

int main(int argc,char* argv[])
{
    if (argc != 2) {
        printf("input error\n");
        exit(0);
    }
    struct sockaddr_in serv_addr;
    signal(SIGINT,sig_proc);
    signal(SIGPIPE,sig_pipe);

    clnt_sock = socket(AF_INET,SOCK_STREAM,0);
    if (clnt_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,argv[1],&serv_addr.sin_addr);
    int ret = connect(clnt_sock,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
    if (ret == -1) {
        printf("connect error:%m\n");
        exit(-1);
    }
    process_conn_client(clnt_sock);
    close(clnt_sock);
}

void sig_proc(int signo)
{
    printf("Catch a exit signal\n");
    close(clnt_sock);
    exit(0);
}

void sig_pipe(ing signo)
{
    printf("Catch a SIGPIPE signal\n");
}

void process_conn_clinet(int clnt_sock)
{
    sszie_t size = 0;
    char buff[1024] = {0};
    while (true) {
        size = read(0,buff,1024);
        if (size > 0) {
            send(clnt_sock,buff,1024,0);
            write(1,buff,size);
        }
    }
}