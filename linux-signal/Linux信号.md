## **Linux 信号**

信号本质上是整数，用户模式下用来模拟硬件中断

什么是硬件中断？先理解为硬件引发的中断。什么是中断？简言之，就是让CPU停下当前干的事转而去处理新的情况。

信号是谁产生的？可以由硬件、内核和进程产生。

例如在终端上用Ctrl+C，可以结束掉当前终端进程，本质上是发送SIGINT信号

如下是一个实例程序，该程序循环打印数字。运行起来，看看Ctrl+C能不能中止它

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

void hand(int val)
{
    printf("val: %d\n",val);
    printf("Do you want to kill me? No way!\n");
}

int main()
{
    signal(SIGINT,hand);
    int n = 0;
    while (true) {
        printf("n:%d\n",n++);
        sleep(1);
    }
    return 0;
}
```

如下所示，现在使用Ctrl+C是没办法中断这个程序的

![](/home/hwx/.config/marktext/images/2022-03-30-19-45-07-image.png)

这里可以用man命令查看一下signal函数的帮助文档

![](/home/hwx/.config/marktext/images/2022-03-30-19-47-08-image.png)

signal关联上了一个信号处理函数，来处理信号

现在修改这个代码，添加一行打印当前进程号，然后打开两个终端，一个终端运行上面的程序，一个终端尝试kill命令来发送信号

![](/home/hwx/.config/marktext/images/2022-03-30-19-53-01-image.png)

kill 要指定参数

![](/home/hwx/.config/marktext/images/2022-03-30-19-53-24-image.png)

这是可行的，结果如上

**信号处理的过程**

进程A在运行，内核、硬件或者其他进程发送信号给进程A。进程A接收到信号后，直接做信号本身规定的对应处理或者做事先注册好的信号处理。如上面signal函数就是注册信号处理，hand函数替换了本身默认的信号处理。当然信号是可以屏蔽的，不作任何处理。

Linux有哪些信号类型

![](/home/hwx/.config/marktext/images/2022-03-30-20-09-48-image.png)

 Linux 信号有64个，分为不可靠信号(非实时，1-31，Unix提供)和可靠信号(32-64，后来扩充的)。又有系统自带的标准信号和用户自定义的信号。

介绍几个命令或函数

信号注册：signal、sigaction

信号发送：kill命令、kill函数、sigqueue

信号屏蔽：sigprocmask

信号集：sigset_t

下面写一个发送信号的程序

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
    int pid = atoi(argv[1]);
    int sig = atoi(argv[2]);

    printf("pid:%d  sig:%d\n",pid,sig);
    kill(pid,sig);
    return 0;
}
```

![](/home/hwx/.config/marktext/images/2022-03-30-20-27-09-image.png)

![](/home/hwx/.config/marktext/images/2022-03-30-20-27-41-image.png)

成功发送了信号

sigaction函数有所不同，功能更多

![](/home/hwx/.config/marktext/images/2022-03-30-20-30-23-image.png)

这里用到了一个结构体

![](/home/hwx/.config/marktext/images/2022-03-30-20-31-32-image.png)

结构体的第一个成员依然是原来的信号处理函数，第二个成员被称作高级信号处理函数，第三个成员用于信号屏蔽，剩下两个暂时还用不到。

这个函数不仅可以发信号、做信号处理，还可以接收信号的同时接收数据

下列代码使用了高级信号处理

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

// 基本信号处理函数
void hand(int n)
{
    printf("基本信号处理函数\n");
}

// 高级信号处理函数
void handler(int n,siginfo_t* siginfo,void* arg)
{
    printf("高级信号处理函数\n");
    printf("n:%d msg:%d\n",n,siginfo->si_int);
}

int main(void)
{
    struct sigaction act = {0};
    struct sigaction old_act = {0};
    act.sa_handler = hand;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    // 高级信号处理
    sigaction(2,&act,&old_act);
    printf("pid:%d\n",getpid());

    int n = 0;
    while (true) {
        printf("n:%d\n",n++);
        sleep(1);
    }
    return 0;
}
```

同时还要实现高级的信号发送，采用sigqueue

![](/home/hwx/.config/marktext/images/2022-03-30-20-47-14-image.png)

这里有个重要的联合体，用来存储数据和信号一起发送

![](/home/hwx/.config/marktext/images/2022-03-30-20-47-58-image.png)

```c
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
```

运行结果如下

![](/home/hwx/.config/marktext/images/2022-03-30-21-03-05-image.png)

![](/home/hwx/.config/marktext/images/2022-03-30-21-02-43-image.png)

可以看到程序接收到了信号和数据

也试试发送其他数据，联合体中还有一个指针类型的成员



最后看看信号屏蔽

这就像手机中黑名单的功能，将不想联系的电话号码拉入黑名单，就可以不会接收到其电话

如下是相关函数

![](/home/hwx/.config/marktext/images/2022-03-30-21-10-08-image.png)

![](/home/hwx/.config/marktext/images/2022-03-30-21-12-47-image.png)

这个函数要设置信号集，设置信号集可以使用专门的函数，如下所示

![](/home/hwx/.config/marktext/images/2022-03-30-21-15-28-image.png)

sigemptyset清空信号集 sigfillset 初始化信号集，将所有信号加进去

sigaddset 往信号集中添加一个信号

sigdelset 从信号集中删除一个信号

sigismember 判断某个信号是否在信号集中

如下是实例代码：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void hand(int n) {
    printf("响应信号\n");
}

int main(void)
{
    sigset_t set,old_set;
    int ret;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    // 前5秒不设置信号屏蔽
    signal(SIGINT,hand);
    sleep(5);
    // 设置信号屏蔽20秒
    if (1 == sigismember(&set,SIGINT)) {
        printf("设置型号屏蔽\n");
        ret = sigprocmask(SIG_BLOCK,&set,&old_set);
        if (0 == ret) printf("设置信号屏蔽成功\n");
        else printf("设置信号屏蔽失败\n");
    }
    sleep(20);

    printf("解除信号屏蔽\n");
    if (1 == sigismember(&set,SIGINT)) {
        printf("解除信号屏蔽\n");
        ret = sigprocmask(SIG_UNBLOCK,&set,&old_set);
        if (0 == ret) printf("设置信号屏蔽成功\n");
        else printf("设置信号屏蔽失败\n");
    }    

    while(1);
    return 0;
}
```

直接运行，可以发现前5秒还在响应ctrl+c的信号，随后20秒则没有反应，20秒过后又恢复响应

![](/home/hwx/.config/marktext/images/2022-03-30-21-34-01-image.png)
