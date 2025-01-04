#pragma once
#include "Epoll.h"
#include <functional>
#include <memory>// 使用只能指针 
/**
 *  每个 事件循环里肯定有一个 epoll 
 */

// 不加这两行会报错
class Channel;
class Epoll;

class EventLoop
{
    private:
        // Epoll * ep_;    // 一个网络服务中 最多有十几个 事件循环， 所以 epoll 使用 栈内存 没有问题
        std::unique_ptr<Epoll> ep_;                       // 每个事件循环只有一个Epoll。
        std::function<void(EventLoop*)> epollTimeOutCallBack_;  // TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
    public:
        EventLoop();
        ~EventLoop();

        // 返回 Epoll
        Epoll* ep();

        // 运行事件循环
        void run();


        void updateChannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。

        // epollTimeOutCallBack_
        void setEpollTimeOutCallBack(std::function<void(EventLoop*)> cb);

        void removeChannel(Channel *ch);                      // 把channel从红黑树上移除
};


