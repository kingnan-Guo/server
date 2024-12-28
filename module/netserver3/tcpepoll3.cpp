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

// 设置非阻塞的IO。
// void setnonblocking3(int fd)
// {
//     fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
// }

int tcpepoll3(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // 创建服务端用于监听的listenfd。
    // int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);// 使用 SOCK_NONBLOCK  创建 非阻塞的套接字。
    // if (listenfd < 0)
    // {
    //     perror("socket() failed"); return -1;
    // }


    Socket serverScoket(createnonblocking());

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

    
    /*
    
    int epollfd = epoll_create(1);        // 创建epoll句柄（红黑树）。

    // 为服务端的listenfd准备读事件。
    struct epoll_event ev;              // 声明事件的数据结构。
    ev.data.fd = listenfd;                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = EPOLLIN;                // 让epoll监视listenfd的读事件，采用水平触发。

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);     // 把需要监视的listenfd和它的事件加入epollfd中。

    struct epoll_event evs[10];      // 存放epoll_wait()返回事件的数组。
    */


    Epoll ep;
    // 让epoll监视listenfd的读事件，采用水平触发。
    ep.addFd(listenfd, EPOLLIN);

    std::vector<epoll_event> evs; // 存放epoll_wait()返回事件的数组。

    while (true)        // 事件循环。
    {
         /*
        int infds = epoll_wait(epollfd, evs, 10, -1);       // 等待监视的fd有事件发生。

        // 返回失败。
        if (infds < 0)
        {
            perror("epoll_wait() failed"); break;
        }

        // 超时。
        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n"); continue;
        }
         */
        

        //  等待监视的fd有事件发生 ,返回容器
        evs = ep.loop();


        // 如果infds>0，表示有事件发生的fd的数量。
        //for (int ii = 0; ii < infds; ii++)       // 遍历epoll返回的数组evs。
        
        // 遍历容器 
        for(auto &ev:evs)
        {
                   
            
            {
                // 如果是客户端连接的fd有事件。
                ////////////////////////////////////////////////////////////////////////
                if (ev.events & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
                {
                    printf("1client(eventfd=%d) disconnected.\n", ev.data.fd);
                    close(ev.data.fd);            // 关闭客户端的fd。
                }                                //  普通数据  带外数据

                else if (ev.events & (EPOLLIN | EPOLLPRI))   // 接收缓冲区中有数据可以读。
                {


                    if (ev.data.fd == listenfd)   // 如果是listenfd有事件，表示有新的客户端连上来。
                    {


                        ////////////////////////////////////////////////////////////////////////
    

                        InetAddress clientaddr;             // 客户端的地址和协议。

                        Socket *clientSock = new Socket(serverScoket.accept(clientaddr));

 

                        printf ("accept client(fd=%d, ip=%s, port=%d ) ok.\n", clientSock->fd(), clientaddr.ip(), clientaddr.port());


                        /*
                        // 为新客户端连接准备读事件，并添加到epoll中。
                        ev.data.fd = clientSock->fd();
                        ev.events = EPOLLIN | EPOLLET;           // 边缘触发。
                        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSock->fd(), &ev);
                      
                        */


                        // 为新客户端连接准备读事件，并添加到epoll中。
                       ep.addFd(clientSock->fd(), EPOLLIN | EPOLLET);           // 边缘触发。

                       


                        ////////////////////////////////////////////////////////////////////////

                    } else {


                        char buffer[1024];
                        while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                        {    
                            bzero(&buffer, sizeof(buffer));
                            ssize_t nread = read(ev.data.fd, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                            if (nread > 0)      // 成功的读取到了数据。
                            {
                                // 把接收到的报文内容原封不动的发回去。
                                printf("recv(eventfd=%d):%s\n", ev.data.fd, buffer);
                                send(ev.data.fd, buffer, strlen(buffer), 0);
                            } 
                            else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                            {  
                                continue;
                            } 
                            else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                            {
                                break;
                            } 
                            else if (nread == 0)  // 客户端连接已断开。
                            {  
                                printf("2client(eventfd=%d) disconnected.\n",ev.data.fd);
                                close(ev.data.fd);            // 关闭客户端的fd。
                                break;
                            }
                        }



                    }
                    
                }

                else if (ev.events & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
                {
                }

                else                                                                    // 其它事件，都视为错误。
                {
                    printf("3client(eventfd=%d) error.\n", ev.data.fd);
                    close(ev.data.fd);            // 关闭客户端的fd。
                }
                ////////////////////////////////////////////////////////////////////////

            }


        }
    }

  return 0;
}


