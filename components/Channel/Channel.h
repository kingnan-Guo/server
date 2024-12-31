#pragma once
#include <sys/epoll.h>
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"


class Epoll; // Epoll.h 中没有包含 Channel.h ，所以这里需要前置声明
class EventLoop; // EventLoop.h 中没有包含 Channel.h ，所以这里需要前置声明

class Channel {
    private:
        int fd_ = -1; // channel 拥有 fd ， channel 与 fd 是 一对一 关系
        // Epoll * ep_ = nullptr; // channel 一定会对应一颗 红黑树 Epoll
        EventLoop *loop_ = nullptr;   // Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个Channel只对应一个EventLoop。
        bool inEpoll_ = false; // channel 是否在 epoll 中, 如果 未添加 调用 epoll_ctl() 的时候用 EPOLL_CTL_ADD 添加，否则 EPOLL_CLT_MOD 修改
        uint32_t events_ = 0; // fd 需要监视的事件， listenfd 和 clientdf 需要监视 EPOLLION ， clientdf 需要监视  EPOLLOUT
        uint32_t revents_ = 0; // fd  已经发生的事件


        // 添加 回调函数的 成员变量 =====
        // 函数的类型是 void() 参数为 空 
        std::function<void()> readCallback_; // fd_ 读事件 的。回调函数
    public:
        // Channel(Epoll* ep, int fd); // 构造函数
        Channel(EventLoop* eLoop, int fd); // 构造函数；    Channel 是 Acceptor 和 Connection 的 下层类 ； 
        ~Channel();                    //   析构函数

        int fd(); // 返回 fd
        void useEt(); // 采用边缘 触发
    
        void enableReading(); // 让 epoll_wait() 监视 fd 的 读事件
        void disableReading(); // 让 epoll_wait() 停止监视 fd 的 读事件

        void setInEpoll(bool inEpoll = true); // 设置 channel 是否在 epoll 中; 默认 把成员变量设置为 true
        bool inEpoll(); // 返回 channel 是否在 epoll 中


        uint32_t events(); // 返回 events_

        void setRevents(uint32_t revents); // 设置 revents_ 为 revents
        uint32_t revents(); // 返回 revents_


        // 事件处理函数 ， epoll_wait() 返回的时候， 执行它 =====
        void handleEvent();


        // 将 客户端连上来 和  连接的客户端的fd有事件 封装成 回调函数 ===== 
        void newConnection(Socket * serverSocket); // 处理新客户端连接请求 ，修改到 Acceptor 类中
        void onMessage();                           // 处理 对端 发送过来的消息

        void setReadCallback(std::function<void()> readCallback); // 设置  回调函数的成员函数。readCallback_


};


