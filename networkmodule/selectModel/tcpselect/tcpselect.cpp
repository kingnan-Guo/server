/* 
/*
 * 程序名：tcpselect.cpp，此程序用于演示采用select模型实现网络通讯的服务端。
 * 作者：吴从周
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include "tcpselect.h"

// 初始化服务端的监听端口。
// 初始化服务端的监听端口。
int initserver(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock < 0)
    {
        perror("socket() failed"); return -1;
    }

    int opt = 1; unsigned int len = sizeof(opt);
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,len);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sock,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
    {
        perror("bind() failed"); close(sock); return -1;
    }

    if (listen(sock,5) != 0 )
    {
        perror("listen() failed"); close(sock); return -1;
    }

    return sock;
}



int mymain(int argc,char *argv[])
{
    if (argc != 2) { printf("usage: ./tcpselect port\n"); return -1; }

    // 初始化服务端用于监听的socket。
    int listensock = initserver(atoi(argv[1]));
    printf("listensock=%d\n",listensock);

    if (listensock < 0) { printf("initserver() failed.\n"); return -1; }


    // select()函数的参数：   
    //      把看监听的socker 和 客户端 连接的 socker  加入到一个集合中

    // 读事件：1）已连接队列中有已经准备好的socket（有新的客户端连上来了）；
    //               2）接收缓存中有数据可以读（对端发送的报文已到达）；
    //               3）tcp连接已断开（对端调用close()函数关闭了连接）。
    // 写事件：发送缓冲区没有满，可以写入数据（可以向对端发送报文）。
    

    // 把客户端监听的 socker 加入到集合中
    fd_set readfds;                         // 需要监视读事件的socket的集合，大小为16字节（1024位）的bitmap。 ； fd_set 的本质 是。大小是 32 位的 整形数组，一个整形有 4 个字节，一个字节有 8 位； int[32]  = 4 * 8 * 32 = 1024 位， 这个是 bitmap 位图， 用 bitmap 表示 socket 的集合
    FD_ZERO(&readfds);                // 初始化readfds，把bitmap的每一位都置为0。
    FD_SET(listensock,&readfds);  // 把服务端用于监听的socket加入readfds。  如果 要把  3 、 4、 5的 socker加入集合，那么 就把bitmap 的 3、4、5 的位置 置为 1。

    int maxfd=listensock;              // readfds中socket的最大值。

    while (true)        // 事件循环。
    {
        // 用于表示超时时间的结构体。
        struct timeval timeout;     
        timeout.tv_sec=10;        // 秒 ； 监视定义 如果监视的窗口 没有任何事情发生  如果超过 10s 就超时 处理， 返回超时 
        timeout.tv_usec=0;        // 微秒。

        fd_set tmpfds = readfds;      // 在select()函数中，会修改bitmap，所以，要把readfds复制一份给tmpfds，再把tmpfds传给select()。
        fd_set tmpfds_write = readfds;
        // 调用select() 等待事件的发生（监视哪些socket发生了事件)。
        /**
         * select () 函数的参数：
         *      maxfd+1 是 告诉  select() 函数， bitmap 的最大值， bitmap 的最大值 是 maxfd+1
         *      &tmpfds 读事件的 bitmap ； 
         *      NULL 写事件的 bitmap
         *      NULL 异常事件的 bitmap； 这个 在监视。普通的 IO 的时候 会用到， 网络编程中 永不倒
         *      0 是 超时时间， 0 表示不超时， 10s 表示 超时 10s ； 也可以传 &timeout 表示 超时时间 是 timeout
         * 
         * 
         * select() 函数的返回值：
         *      如果返回值 < 0  ， 返回值是 -1 ，表示调用select()失败。 如果 失败 会 复制。ERRORS 全局变量 ， EBADF 、EINTR （收到了信号被信号中断）、EINAVL （参数错误）、ENOMENM。内存不足
         *      如果返回值 == 0，表示select()超时。
         *      如果返回值 > 0，表示有事件发生，返回值存放了已发生事件的个数。 
         *              如果 接收到事件发生那么返回 已 事件发生的个数 ， 并且会修改 tmpfds ;
         *              假设 传给 select 的 bitmap里面包含  3、4、5、6；  要监视 bitmap 中包含 3、4、5、6 ，如果只有 3 和 6 的 socker 有事件， 在 select 函数里面 会把 4 、5  清空， 只留下 3、 6
         *              在程序中，遍历 select 的 返回的 bitmap ，就可以知道哪些 sokcer 有事件
         * 
         * 
         * 
         */
        int infds = select(maxfd+1, &tmpfds, &tmpfds_write, NULL, &timeout); 

        // 如果infds<0，表示调用select()失败。
        if (infds < 0)
        {

            perror("select() failed"); 
            break;
        }

        // 如果infds==0，表示select()超时。
        if (infds==0)
        {
            printf("select() timeout.\n"); continue;
        }

        // 如果infds>0，表示有事件发生，infds存放了已发生事件的个数。
        for (int eventfd = 0; eventfd <= maxfd; eventfd++)
        {
            // FD_ISSET 判断 bitmap 中的第 eventfd 位 是否为 1 ， 如果为 1 表示有事件发生
            if (FD_ISSET(eventfd, &tmpfds)==0) continue;   // 如果eventfd在bitmap中的标志为0，表示它没有事件，continue

            // 如果发生事件的是listensock，表示已连接队列中有已经准备好的socket（有新的客户端连上来了）。
            // 监听的socker 只用来连接 不会用来通信
            if (eventfd == listensock)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int clientsock = accept(listensock, (struct sockaddr*) &client, &len);// 从已连接队列中获取一个 客户端连接 
                if (clientsock < 0) { 
                    perror("accept() failed"); 
                    continue; 
                }

                printf ("accept client(socket=%d) ok.\n",clientsock);

                FD_SET(clientsock,&readfds);                      // 把bitmap中新连上来的客户端的标志位置为1。 改变 原始数据 ，因为传到 select 里面的是  tmpfds 副本，所以 tmpfds 改变后要改变原始数据

                if (maxfd<clientsock) maxfd=clientsock;    // 更新maxfd的值。因为新连接一个客户端进来 ,那么 句柄就会自增 1 ， 但是如果 添加个 3 个socker 但是 第一socker 第二 socker 个都 close 了后,那么 新增的 句柄还是 加一吗？ 第三个socker 的句柄 是 5 ，但是 3 、4 的句柄已经 close 了，那么 maxfd 还是 5 吗？ 不是， maxfd 应该是 3 ，因为 3 、4 已经 close 了，所以 maxfd 应该是 3
            }
            else
            {
                // 如果是客户端 连接 的socke有事件，表示接收缓存中有数据可以读（对端发送的报文已到达），或者有客户端已断开连接。
                char buffer[1024];                      // 存放从接收缓冲区中读取的数据。
                memset(buffer, 0, sizeof(buffer));      // 把buffer清零。
                if (recv(eventfd, buffer, sizeof(buffer), 0) <= 0) // 从接收缓存区中读取数据 ； 如果 = 0 表示 对方已经断开连接
                {
                    // 如果客户端的连接已断开。
                    printf("client(eventfd=%d) disconnected.\n", eventfd);

                    close(eventfd);                         // 关闭客户端的socket

                    FD_CLR(eventfd, &readfds);     // 把bitmap中已关闭客户端的标志位清空。
          
                    if (eventfd == maxfd)              // 重新计算maxfd的值，注意，只有当eventfd==maxfd时才需要计算。
                    {
                        for (int ii = maxfd;ii > 0;ii--)    // 从后面往前找。
                        {
                            if (FD_ISSET(ii, &readfds))     // 如果ii在bitmap中的标志为1，表示ii是有效socket，
                            {
                                maxfd = ii; 
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // 如果客户端有报文发过来。
                    printf("recv(eventfd=%d):%s\n",eventfd,buffer);

                    // 把接收到的报文内容原封不动的发回去。
                    send(eventfd, buffer, strlen(buffer),0);
                }
            }
        }



        for (int eventfd = 0; eventfd <= maxfd; eventfd++)
        {
            // FD_ISSET 判断 bitmap 中的第 eventfd 位 是否为 1 ， 如果为 1 表示有事件发生
            if (FD_ISSET(eventfd, &tmpfds_write)==0) continue;   // 如果eventfd在bitmap中的标志为0，表示它没有事件，continue
            // printf(" 可以写 ");
        }

        
    }

    return 0;
}


// select 可以监视多个 socker 把需要监视的socker 作为参数 ， 如果被监视的socker 没有事件发生那么 会阻塞等待，如果发生了事件 他会 返回


int TCP_SELECT_MAIN(int argc,char *argv[]){
    mymain(argc, argv);
}
