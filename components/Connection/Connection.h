#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Connection{
    private:
        EventLoop* loop_;    // Connection 对应的事件循环， 在构造函数中传入, 从外面传过来的 
        Socket* clientScoket_;  // 与 客户端 通信 的 socket； 虽然 是传入进来的值 ，但是 属于 Connection 的成员变量，所以 需要在析构函数中删除 
        Channel* clientChannel_;  // Connection 对应的 的 channel ，在构造函数中创建

        // 添加回调函数
        std::function<void(Connection*)> closeCallBack_;  // TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
        std::function<void(Connection*)> errorCallBack_;  //TCP 连接错误的 回调函数， 提供 Channel 回调

    public:
        Connection(EventLoop* loop, Socket* clientScoket);
        ~Connection();


        int fd() const;
        std::string ip() const;
        uint16_t port() const;




        // 

        void closeCallBack();   // TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
        void errorCallBack();  //TCP 连接错误的 回调函数， 提供 Channel 回调

        // 设置 回调函数
        void setCloseCallBack(std::function<void(Connection*)> closeCallBack);
        void setErrorCallBack(std::function<void(Connection*)> errorCallBack);

};


