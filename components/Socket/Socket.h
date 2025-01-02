#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"


int createnonblocking();

class Socket{
    private:
        const int fd_; // Scoket 的 fd ， 在构造函数中传进来

        // 把客户端的 ip 和端口信息保存下来
        std::string ip_; // 如果是 listenfd ，存放服务端监听的 ip， 如果是客户端连接的 fd ，存放 对端的ip
        uint16_t port_; // 如果是 listenfd ，存放服务端监听的 port， 如果是客户端连接的 fd ，存放 对端的port
    public:
        Socket(int fd);
        ~Socket();

        int fd() const;
        void bind(const InetAddress &servaddr);
        void listen(int byte = 128);
        int accept(InetAddress& clientaddr);
        
        void setIpPort(const std::string &ip, uint16_t port);   // 设置 ip 和 port

        void setreuseaddr(bool on);       // 设置SO_REUSEADDR选项，true-打开，false-关闭。
        void setreuseport(bool on);       // 设置SO_REUSEPORT选项。
        void settcpnodelay(bool on);     // 设置TCP_NODELAY选项。
        void setkeepalive(bool on);       // 设置SO_KEEPALIVE选项。


        //  用于 获取 客户端的 ip 和端口信息
        std::string ip() const;
        uint16_t port() const;
};


