#pragma once
#include <sys/epoll.h>
#include "Epoll.h"
#include "inetAddress.h"
#include "socket.h"
#include "Channel.h"
#include "EventLoop.h"



class TcpServer{
    private:
        EventLoop loop_; // 一个 Tcp Server可以有多个事件循环，现在是单线程，暂时只用一个事件循环

    public:
        // 需要把 ip 和 端口 传进来
        TcpServer(const std::string &ip, const uint16_t port);
        ~TcpServer();

        // 运行时间循环
        void start();
};