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

#include "TimeStamp.h"
#include <sys/timerfd.h>      // 定时器需要包含这个头文件。
#include <map>
#include "Connection.h"

/**
 *  每个 事件循环里肯定有一个 epoll 
 */

// 不加这两行会报错
class Channel;
class Epoll;
class Connection;
//定义智能指针 别名， 智能指针 spConnection，防止内存泄漏
using spConnection = std::shared_ptr<Connection>;

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

        // 定时器相关
        int timerFd_; // 时间fd
        std::unique_ptr<Channel> timerChannel_; // 定时器的 channel 用来向 epoll 注册 定时器 fd
        bool isMainLoop_; // 是否时 true 主事件循环， false 表示 从事件循环
        std::map<int, spConnection> ConnectionMap_; // 定时器 map， key 是 timerfd， value 是 Connection, 存放运行在该事件循环上全部的Connection对象。
        std::mutex connectionsMapMutex_;   // 保护 ConnectionMap_ 的互斥锁。

        // 从 map 中删除 Connection 的 回调函数
        std::function<void(int)> timerCallBack_;    // 删除 TcpServer中 的 Connection 对象， 将被设置为TcpServer::removeConnection()

        // 超时事件间隔  参数
        int timeVl_; // 定时器 闹钟 时间间隔
        int timeOut_; // 定时器 超时时间
    
    public:
        EventLoop(bool isMainLoop, int timeVl = 30, int timeOut = 60);
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


        // 定时器相关： 闹钟响时 执行的函数
        void handleTimer();

        //添加 Connection 到  ConnectionMap_ 中的函数
        void newConnection(spConnection conn);

        // 设置 回调函数的 成员函数 // 将被设置为TcpServer::removeConnection()
        void setTimerCallBack(std::function<void(int)> cb);

};


