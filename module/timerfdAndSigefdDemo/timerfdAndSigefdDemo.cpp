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


#include <sys/timerfd.h>      // 定时器需要包含这个头文件。
#include <signal.h>                // 信号需要包含这个头文件。
#include <sys/signalfd.h>     // 信号fd需要包含这个头文件。

// 设置非阻塞的IO。
void setnonblocking5(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int timerfdAndSigefdDemo(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // 创建服务端用于监听的listenfd。
    int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed"); return -1;
    }

    // 设置listenfd的属性，如果对这些属性不熟悉，百度之。
    int opt = 1; 
    //  为监听套接字 listenfd 启用 SO_REUSEADDR 选项。 这样可以在服务器程序因异常退出后，立即重新启动绑定到同一个地址和端口，而无需等待系统释放端口。
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。
    // 禁用 Nagle 算法，让数据包在调用 send() 或类似函数时立即发送，而不再延迟合并。提高了数据传输的实时性，但可能会增加小包的发送数量，从而影响网络效率。
    setsockopt(listenfd,SOL_SOCKET,TCP_NODELAY   ,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。
    /**
     * 支持多进程/多线程服务器：
     *    允许多个进程或线程绑定到同一个地址和端口。
     *    在一个多线程服务器中，每个线程可以独立创建一个监听套接字，并监听同一个端口。
     *      
     * 负载均衡：
     *      当多个进程或线程绑定到同一个端口时，内核会将新连接按照负载均衡策略分发给这些进程/线程
     * 
     * 解决端口占用冲突：
     *      在默认情况下，同一个端口只能由一个套接字绑定。如果设置了 SO_REUSEPORT，多个套接字可以同时绑定到同一个端口，而不会出现 bind: Address already in use 错误。
     * 
     */
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt));    // 有用，但是，在Reactor中意义不大。
    /**
     * 长连接应用：
     *      常用于长时间保持连接的场景（如 WebSocket、REST API、数据库连接等），可以检测远程主机是否存活
     * 
     * 防止资源浪费：
     *      如果对端异常断开而未通知本地应用，Keep-Alive 可以及时检测并释放相关资源。
     * 
     * 高可靠性场景
     *         确保应用及时感知连接中断，以便快速重试或切换到备用连接。
     * 
     */
    
    setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE   ,&opt,static_cast<socklen_t>(sizeof opt));    // 可能有用，但是，建议自己做心跳。

    setnonblocking5(listenfd);    // 把服务端的listenfd设置为非阻塞的。

    struct sockaddr_in servaddr;                                  // 服务端地址的结构体。
    servaddr.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);      // 服务端用于监听的ip地址。
    servaddr.sin_port = htons(atoi(argv[2]));               // 服务端用于监听的端口。

    if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
    {
        perror("bind() failed"); close(listenfd); return -1;
    }

    if (listen(listenfd,128) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); close(listenfd); return -1;
    }

    int epollfd=epoll_create(1);        // 创建epoll句柄（红黑树）。

    // 为服务端的listenfd准备读事件。
    struct epoll_event ev;              // 声明事件的数据结构。
    ev.data.fd=listenfd;                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events=EPOLLIN;                // 让epoll监视listenfd的读事件，采用水平触发。

    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);     // 把需要监视的listenfd和它的事件加入epollfd中。

    struct epoll_event evs[10];      // 存放epoll_wait()返回事件的数组。





    /////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////



    /**
     * 把定时器 加入到 epoll
     * 
     * 新版本的Linux内核支持 epoll timerfd，它允许将定时器事件添加到 epoll 实例中，从而实现高效的事件驱动定时器功能。
     * 
     * timerfd_create() 函数创建一个 timerfd，并将其添加到 epoll 实例中，以便在定时器到期时接收通知。
     *      paranms : 
     *          clockid 指定了定时器的时钟源，可以是 CLOCK_REALTIME （系统时钟）或 CLOCK_MONOTONIC （单调时钟） (常用)。
     *           flags 可以指定定时器的行为，例如一次性定时器或周期性定时器。
     *                  参数
     *                       0：创建一个一次性定时器，定时器到期后不再自动重置。
     *                       TFD_NONBLOCK ：将文件描述符设置为非阻塞模式。
     *                       TFD_CLOEXEC ：在执行 exec() 系统调用时关闭文件描述符。
     *                          
     *      返回值：
     *           成功时，timerfd_create() 返回一个文件描述符，用于访问定时器。如果失败，则返回 -1，并设置 errno。
     * 
     * 
     * timerfd_settime() 函数设置定时器的初始值和周期。参数 flags 可以指定定时器的行为，例如一次性定时器或周期性定时器。
     *      params:
     *               timerid 是 timerfd_create() 返回的文件描述符。
     *               itimerspec 结构体指定了定时器的初始值和周期。itimerspec 结构体包含两个 itimerspec_val 结构体，分别指定了定时器的初始值和周期。
     *               flags 可以指定定时器的行为，例如一次性定时器或周期性定时器。
     *      返回值：
     *             成功时，timerfd_settime() 返回 0。如果失败，则返回 -1，并设置 errno。
     * 
     * 
     * 
    */

    // 创建一个 timerfd。
    int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK );

    // 设置定时器的初始值和周期。
    struct itimerspec timeout;
    // 注释 
    memset(&timeout, 0, sizeof(struct itimerspec));

    // 设置时间
    timeout.it_value.tv_sec = 5; // 5秒后触发
    timeout.it_value.tv_nsec = 0;// 纳秒

    // 开始计时 alarm(5)
    timerfd_settime(timerFd, 0, &timeout, NULL);
    // 将 timerfd 添加到 epoll 实例中。
    ev.data.fd = timerFd; // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = EPOLLIN; // 让epoll监视timerfd的读事件，采用水平触发。

    // 把定时器 fd 加入 epoll
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timerFd, &ev); 


    


    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    /**
     * 把信号加入 epoll
     * 
     * 
     * 
    */

    // 创建一个信号描述符。
    sigset_t sigset;
    // 初始化（ 清空 ）信号集！！！
    sigemptyset(&sigset);
    // 把 SIGINT 添加到信号集中。
    sigaddset(&sigset, SIGINT);
    // 把SIGTERM信号加入信号集。
    sigaddset(&sigset, SIGTERM);

    // 对当前进程屏蔽信号集 （当前进程将收不到 信号集 中的信号）
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // 创建 信号集的 fd
    int sigFd = signalfd(-1, &sigset, 0);

    // 为 SIGINT 和 SIGTERM 准备读事件，并添加到epoll中。
    ev.data.fd = sigFd; // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = EPOLLIN; // 让epoll监视timerfd的读事件，采用水平触发。

    // 把信号 fd 加入 epoll
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sigFd, &ev);


    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////










    while (true)        // 事件循环。
    {
        int infds=epoll_wait(epollfd,evs,10,-1);       // 等待监视的fd有事件发生。

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

        // 如果infds>0，表示有事件发生的fd的数量。
        for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
        {
            if (evs[ii].data.fd == listenfd)   // 如果是listenfd有事件，表示有新的客户端连上来。
            {


                ////////////////////////////////////////////////////////////////////////
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(listenfd,(struct sockaddr*)&clientaddr,&len);
                setnonblocking5(clientfd);         // 客户端连接的fd必须设置为非阻塞的。

                printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientfd,inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));

                // 为新客户端连接准备读事件，并添加到epoll中。
                ev.data.fd=clientfd;
                ev.events=EPOLLIN|EPOLLET;           // 边缘触发。
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
                ////////////////////////////////////////////////////////////////////////

            }
            // 收到 定时器fd 的事件。
            else if(evs[ii].data.fd == timerFd) {
                printf("定时器fd有事件发生\n");

                // 重新计时，这样就会开始下一次循环 
                timerfd_settime(timerFd, 0, &timeout, 0);

                // 读取定时器fd，这样就不会阻塞。
                uint64_t exp;
                read(timerFd, &exp, sizeof(exp));
                printf("读取 定时器 = %d \n", exp);

                printf("在这里编写处理定时器的代码 。。。。。。。。\n");
                continue;
            }
            // 收到 信号fd 的事件。
            else if(evs[ii].data.fd == sigFd) {

                struct signalfd_siginfo sigInfo;

                // 读取信号fd，这样就不会阻塞。
                int s = read(sigFd, &sigInfo, sizeof(struct signalfd_siginfo));
                printf("收到了信号 = %d \n", sigInfo.ssi_signo);
                
                // 在这里编写处理信号的代码 。。。。。。。。。
                continue;

            }

            else  // 如果是客户端连接的fd有事件。
            {

                ////////////////////////////////////////////////////////////////////////
                if (evs[ii].events & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
                {
                    printf("1client(eventfd=%d) disconnected.\n",evs[ii].data.fd);
                    close(evs[ii].data.fd);            // 关闭客户端的fd。
                }                                //  普通数据  带外数据
                else if (evs[ii].events & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
                {
                    char buffer[1024];
                    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {    
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)      // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n",evs[ii].data.fd,buffer);
                            send(evs[ii].data.fd,buffer,strlen(buffer),0);
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
                            printf("2client(eventfd=%d) disconnected.\n",evs[ii].data.fd);
                            close(evs[ii].data.fd);            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
                else if (evs[ii].events & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
                {
                }
                else                                                                    // 其它事件，都视为错误。
                {
                    printf("3client(eventfd=%d) error.\n",evs[ii].data.fd);
                    close(evs[ii].data.fd);            // 关闭客户端的fd。
                }
                ////////////////////////////////////////////////////////////////////////

            }
        }
    }

  return 0;
}


//  ./Reactor 127.0.0.1  8080

// killall -9 Reactor 停止进程

// killall -2 Reactor 发送信号2，表示SIGINT，ctrl+c