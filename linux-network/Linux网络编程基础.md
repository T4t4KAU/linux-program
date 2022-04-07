## Linux网络编程基础

这里对计算机网络方面的内容就只作简单的介绍，不牵涉太多学术化的概念。

*参考书目：《Linux网络编程》、《Linux/Unix系统编程手册》*、《计算机网络:自顶而下的方法》

#### 前置知识

OSI七层网络模型、TCP/IP协议栈

socket是一种IPC方法，允许同一主机或使用网络连接起来的不同主机上的应用程序之间交换数据。传输层有TCP协议和UDP协议，这个网上有很多资料。

TCP协议 编程模型：

| 服务端      | 客户端        |
| -------- | ---------- |
| 创建socket | 创建socket   |
| 确定服务器地址簇 | 获取服务器地址协议簇 |
| 绑定       |            |
| 监听       |            |
| 接收连接     | 连接服务器      |
| 通信       | 通信         |
| 断开连接     | 断开连接       |

UDP协议 编程模型

| "服务端"      | "客户端"      |
| ---------- | ---------- |
| 创建socket   | 创建socket   |
| 确定服务端地址协议簇 | 获取客户端地址协议簇 |
| 绑定         |            |
| 通信         | 通信         |

创建socket采用socket函数

![](/home/hwx/.config/marktext/images/2022-04-05-14-18-42-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-14-22-44-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-14-23-01-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-14-24-24-image.png)

#### TCP网络编程

将socket函数的第一个参数domain(通信域)指定为AF_INET，第二个参数是socket类型，这里传入SOCK_STREAM，流式套接字。因为只使用一种协议，这里将第三个参数设置为0

创建完套接字后，要对sockaddr_in结构体进行初始化，struct sockaddr是通用地址结构。

使用bind函数进行绑定，将一个socket绑定到一个地址上

![](/home/hwx/.config/marktext/images/2022-04-05-16-46-06-image.png)

使用listen函数进行监听

_![](/home/hwx/.config/marktext/images/2022-04-05-16-50-03-image.png)

使用accept函数进行接收套接字

![](/home/hwx/.config/marktext/images/2022-04-05-16-50-31-image.png)

使用recv和send函数可以收发数据

![](/home/hwx/.config/marktext/images/2022-04-05-18-07-37-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-18-07-52-image.png)

服务端代码如下

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int serv_sock,clnt_sock;

void hand() {
    // 断开连接
    close(clnt_sock);
    close(serv_sock);
    printf("bye\n");
    exit(0);
}

int main(int argc,char* argv[])
{
    if (argc != 3) {
        printf("input error\n");
        exit(-1);
    }

    signal(2,hand);

    // 创建socket
    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if (serv_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    // 创建套接字地址协议簇
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // IP地址 字符串->点分
    serv_addr.sin_port = htons(atoi(argv[2])); // 转字节序

    // 绑定
    int ret = bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        close(serv_sock);
        exit(-2);
    }
    printf("bind success\n");

    // 监听
    ret = listen(serv_sock,10);
    if (-1 == ret) {
        printf("listen error:%m\n");
        close(serv_sock);
        exit(-2);
    }
    printf("listen success\n");

    struct sockaddr_in clnt_addr = {0};
    int len = sizeof(clnt_addr);
    // 接收连接
    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&len);
    if (clnt_sock == -1) {
        printf("accept error:%m");
        close(serv_sock);
        exit(-2);
    }
    printf("accept success\n");
    printf("%s is connecting\n",inet_ntoa(clnt_addr.sin_addr));

    // 通信
    char buff[256] = {0};
    while (true) {
        ret = recv(clnt_sock,buff,256,0);
        if (ret > 0) {
            buff[ret] = 0;
            printf(">> %s\n",buff);
        }
    }
    return 0;
}
```

要注意的是accept函数是一个阻塞函数，没有连接时，就会卡在那等待。

客户端调用connect连接服务器

![](/home/hwx/.config/marktext/images/2022-04-06-08-27-43-image.png)

客户端代码如下

```c
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
```

先运行服务端，再运行客户端，运行结果如下：

![](/home/hwx/.config/marktext/images/2022-04-05-18-05-55-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-18-06-06-image.png)

将上述过程简单描述就是

下面简单实现一个文件传输

接收端是服务器，发送端是客户端。流程如下：

| 服务端           | 客户端        |
| ------------- | ---------- |
| 等待客户端发送       | 发送文件名      |
| 接收文件名并创建文件    | 获取并发送文件大小  |
| 接收文件大小        | 打开文件准备读取发送 |
| 循环接收并写入数据     | 循环读取并发送    |
| 接收数据完成关闭文件和连接 | 发送完成并关闭文件  |

如下是接收端的代码

```c
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

int serv_sock,clnt_sock;

int main(void) 
{
    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if (serv_sock == -1) {
        printf("socket error:%m\n");
        exit(-1);
    }
    printf("socket success\n");

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ADDR);
    serv_addr.sin_port = htons(PORT);

    int ret = bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret == -1) {
        printf("bind error:%m\n");
        exit(-1);
    }
    printf("bind success\n");

    ret = listen(serv_sock,10);
    if (ret == -1) {
        printf("listen error:%m\n");
        exit(-1);
    }   
    printf("listen success\n");

    struct sockaddr_in clnt_addr = {0};
    int len = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&len);
    if (clnt_sock == -1) {
        printf("accept error:%m\n");
        exit(-1);
    }
    printf("accept success\n");
    printf("%s is connecting\n",inet_ntoa(clnt_addr.sin_addr));

    char buff[1024] = {0}; // 缓冲区 存放文件内容
    char name[256] = {0};  // 文件名
    int size = 0;
    // 先接收文件名和文件大小
    ret = recv(clnt_sock,name,255,0);
    if (ret > 0) {
        name[ret] = 0;
        printf("File name: %s\n",name);
    }
    ret = recv(clnt_sock,(char*)&size,4,0);
    if (ret == 4) {
        printf("File size: %d\n",size);
    }

    int count = 0; // 已经接收的大小
    int fd = open("./recv.txt",O_CREAT|O_WRONLY,0666); // 打开文件
    while (true) {
        ret = recv(clnt_sock,buff,1024,0);
        if (ret > 0) {
            write(fd,buff,ret);
            count += ret;
            printf("recv size: %d\n",count);
            if (count >= size) 
                break;
        }
        else
            break;
    }

    close(fd);
    sleep(1);
    printf("receive ok\n");
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
```

如下是发送端

```c
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

    send(clnt_sock,name,sizeof(name),0);
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
```

![](/home/hwx/.config/marktext/images/2022-04-05-19-41-00-image.png)

这里将上级目录中的一个test.txt文本文件发送，服务端保存在本地为recv.txt

运行结果

![](/home/hwx/.config/marktext/images/2022-04-05-19-40-05-image.png)

![](/home/hwx/.config/marktext/images/2022-04-05-19-40-18-image.png)

已经成功保存

下面介绍UDP

#### UDP网络编程

在socket函数中，协议类型换成SCOK_DGRAM

如下是服务端代码，只要做套接字和绑定操作

```c
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
```

如下是客户端代码

```c
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
```

![](/home/hwx/.config/marktext/images/2022-04-06-19-03-52-image.png)

![](/home/hwx/.config/marktext/images/2022-04-06-19-04-02-image.png)

#### 数据IO

上述用到了一些IO函数，Linux中主要的IO函数有read、write、recv、send、recvmsg、sendmsg、readv、writev。

使用recv函数接收数据，从套接字中接收数据放到缓冲区的buff中，buf的长度为len，操作方式由flags指定，第一个参数是套接字接口文件描述符，由操作系统调用socket返回。第二个参数buf是一个指针，指向接收网络数据的缓冲区。第三个参数len表示接收缓冲区的大小，以字节为单位。

#### 多选选择模型

该模型的核心是select函数，监视描述符集合

![](/home/hwx/.config/marktext/images/2022-04-07-16-00-18-image.png)
