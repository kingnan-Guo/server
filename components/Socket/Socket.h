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
    public:
        Socket(int fd);
        ~Socket();

        int fd() const;
        void bind(const InetAddress &servaddr);
        void listen(int byte = 128);
        int accept(InetAddress& clientaddr);

        void setreuseaddr(bool on);       // 设置SO_REUSEADDR选项，true-打开，false-关闭。
        void setreuseport(bool on);       // 设置SO_REUSEPORT选项。
        void settcpnodelay(bool on);     // 设置TCP_NODELAY选项。
        void setkeepalive(bool on);       // 设置SO_KEEPALIVE选项。
};


