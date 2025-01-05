#pragma once
#include "Epoll.h"
#include <functional>
#include <memory>// 使用只能指针 
#include <sys/syscall.h>
#include <unistd.h>
#include <queue>
#include <mutex> // 锁

#include  <iostream>

#include <sys/eventfd.h>
#include <sys/syscall.h>
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
        pid_t threadId_; //  事件循环 所在 线程的id， 要在运行 事件循环 run() 中 初始化 

        // 创建一个任务队列 
        std::queue<std::function<void()>> taskQueue_; // 事件循环选从 被 eventFd 唤醒后执行的任务 队列

        //
        std::mutex mutex_; // 任务队列  同步 互斥锁

        //生命 eventfd 构造函数呢
        int wakeupFd_; // eventfd 用来 唤醒 事件循环

        std::unique_ptr<Channel> wakeChannel_; // 用来监听 eventfd 的 channel
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

        // 判断当前线程 是否 为事件循环线程
        bool isInLoopThread();

        // 把任务 添加到 队列中 
        void queueInLoop(std::function<void()> fn);

        // 唤醒 事件循环的函数， 用 eventfd 唤醒事件 循环线程
        void wakeup();

        // 增加一个 事件 循环 线程 被eventfd 唤醒 ， 被唤醒后 执行的 函数
        void handleWakeUp();
};


