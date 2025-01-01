#include "Socket.h"


// 创建非阻塞 的 socket
int createnonblocking(){
    // 使用 SOCK_NONBLOCK  创建 非阻塞的套接字。
     int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
     if(listenfd < 0){
        printf("%s:%s:%d listen socket create error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); 
        exit(-1);
     }
     return listenfd;
}




// 给 fd_  赋值
Socket::Socket(int fd): fd_(fd){
}

// 关闭 fd_
Socket::~Socket() {
    ::close(fd_);
}


int Socket::fd() const                              // 返回fd_成员。
{
    return fd_;
}


void Socket::setreuseaddr(bool on){
    int optval = on ? 1 : 0;
    //  为监听套接字 listenfd 启用 SO_REUSEADDR 选项。 这样可以在服务器程序因异常退出后，立即重新启动绑定到同一个地址和端口，而无需等待系统释放端口。
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));    // 必须的。
}

void Socket::setreuseport(bool on){
    int optval = on ? 1 : 0;
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
    setsockopt(fd_,SOL_SOCKET, SO_REUSEPORT ,&optval, static_cast<socklen_t>(sizeof optval));    // 有用，但是，在Reactor中意义不大。
  
}

void Socket::settcpnodelay(bool on){
    int optval = on ? 1 : 0;
    // 禁用 Nagle 算法，让数据包在调用 send() 或类似函数时立即发送，而不再延迟合并。提高了数据传输的实时性，但可能会增加小包的发送数量，从而影响网络效率。
    setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));    // 必须的。
    
}

void Socket::setkeepalive(bool on){
    int optval = on ? 1 : 0;
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
    
    setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t> (sizeof optval));    // 可能有用，但是，建议自己做心跳。

}





void Socket::bind(const InetAddress &servaddr){
    if (::bind(fd_, servaddr.addr(), sizeof(servaddr)) < 0 )
    {
        perror("bind() failed");
        close(fd_);
        exit(EXIT_FAILURE);
    }

    // 监听的 fd
    ip_ = servaddr.ip();
    port_ = servaddr.port();
}




void Socket::listen(int byte){
    if (::listen(fd_, byte) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); 
        close(fd_); 
        exit(EXIT_FAILURE);
    }
}



int Socket::accept(InetAddress& clientaddr){
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = ::accept4(fd_, (struct sockaddr*) &peeraddr, &len, SOCK_NONBLOCK);

    clientaddr.setaddr(peeraddr);



    // 客户端连接 的 fd 
    ip_ = clientaddr.ip();
    port_ = clientaddr.port();

    return clientfd;
}





std::string Socket::ip() const                              // 返回ip_成员。
{
    return ip_;
}

uint16_t Socket::port() const                              // 返回port_成员。
{
    return port_;
}