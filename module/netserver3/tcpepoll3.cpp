/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
 * 作者：吴从周
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include "tcpepoll3.h"
#include "inetAddress.h"
#include "socket.h"
#include "Epoll.h"



int tcpepoll3(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }
    


    Socket serverScoket(createnonblocking());

    // 创建服务端用于监听的listenfd。
    int listenfd = serverScoket.fd();

    // struct sockaddr_in servaddr;                                  // 服务端地址的结构体。
    // servaddr.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    // servaddr.sin_addr.s_addr = inet_addr(argv[1]);      // 服务端用于监听的ip地址。
    // servaddr.sin_port = htons(atoi(argv[2]));               // 服务端用于监听的端口。

    // 初始化 InetAddress ,调用构造函数，传入  ip地址和端口号。
    // InetAddress servaddr = InetAddress (argv[1], atoi(argv[2]));
    InetAddress servaddr(argv[1], atoi(argv[2]));

    serverScoket.setreuseaddr(true);
    serverScoket.settcpnodelay(true);
    serverScoket.setreuseport(true);
    serverScoket.setkeepalive(true);

    serverScoket.bind(servaddr);
    serverScoket.listen();

    


    // 创建epoll句柄（红黑树）。
    Epoll ep;
    // 让epoll监视listenfd的读事件，采用水平触发。


    Channel* servChannel = new Channel(&ep, listenfd);

    // 指定回调函数 
    servChannel->setReadCallback(
        /**
         * 回调函数，当有新的客户端连接时，此函数会被调用。
         * @param &Channel::newConnection       Channel对象中的成员函数。
         * @param servChannel                   服务端用于监听的Channel对象。
         *  @param &serverScoket                newConnection 的传值参数。
         * 
         * 
         */
        std::bind(&Channel::newConnection, servChannel, &serverScoket) // 绑定函数
    );

    // 启用 读事件
    servChannel->enableReading();



    while (true)        // 事件循环。
    {

        
        std::vector<Channel *> channels; // 存放epoll_wait()返回事件的数组。
        //  
        channels = ep.loop();



        // 遍历容器 
        for(auto &ch:channels)
        {


            ch->handleEvent();

        }
    }

  return 0;
}


