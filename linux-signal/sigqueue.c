#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{  
    int pid = atoi(argv[1]);
    int sig = atoi(argv[2]);
    printf("pid:%d  sig:%d\n",argv[1],argv[2]);
    union sigval u;
    u.sival_int = 12345678;
    sigqueue(pid,sig,u);
    return 0;
}