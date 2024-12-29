#pragma once
#include <sys/epoll.h>
#include "Epoll.h"
#include "inetAddress.h"
#include "socket.h"

/**
 *  每个 事件循环里肯定有一个 epoll 
 */


class EventLoop
{
    private:
        Epoll* ep_;
    public:
        EventLoop();
        ~EventLoop();

        // 返回 Epoll
        Epoll* ep();

        // 运行事件循环
        void run();
};


