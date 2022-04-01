#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("This is parent process,pid: %d\n",getpid());
    sleep(2);
    printf("Create child process\n");
    int ret = fork();
    int n = 10;
    if (ret) {
        printf("This is parent process: %d\n",getpid());
        while(1) {
            printf ("n = %d\n",n++);
            sleep(1);
        }
    }
    else {
        printf("This is child process: %d\n",getpid());
        while(1) {
            printf("n = %d\n", n += 2);
            sleep(1);
        }
    }
    return 0;
}