#include <stdio.h>
#include <unistd.h>

int main(void) {
    if (fork()) {
        printf("create child process\n");
        wait(0);
    }
    else {
        for(int i=0;i<5;i++) {
            printf("child process: %d\n",i);
            sleep(1);
        }
    }
    printf("end parent process\n");
    return 0;
}