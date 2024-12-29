#pragma once
#include "Epoll.h"
/**
 *  每个 事件循环里肯定有一个 epoll 
 */

// 不加这两行会报错
class Channel;
class Epoll;

class EventLoop
{
    private:
        Epoll * ep_;
    public:
        EventLoop();
        ~EventLoop();

        // 返回 Epoll
        Epoll* ep();

        // 运行事件循环
        void run();


        void updateChannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
};


