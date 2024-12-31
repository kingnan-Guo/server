#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
    private:
        EventLoop* loop_;    // Acceptor 对应的事件循环， 在构造函数中传入, 从外面传过来的 
        Socket* serverScoket_;  // 服务端用于监听socket ，在构造函数中 创建
        Channel* acceptChannel_;  // Acceptor 对应的 的 channel ，在构造函数中创建

    public:
        Acceptor(EventLoop* loop, const std::string &ip, uint16_t port);
        ~Acceptor();
};


