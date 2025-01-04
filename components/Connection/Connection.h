#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>// 使用只能指针 
#include <atomic>

class Connection;
//定义只能指针 别名， 把 普通指针 Connection*  改为 智能指针 spConnection，防止内存泄漏
using spConnection=std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
    
    private:
        
        EventLoop* loop_;    // Connection 对应的事件循环， 在构造函数中传入, 从外面传过来的 
        Socket* clientScoket_;  // 与 客户端 通信 的 socket； 虽然 是传入进来的值 ，但是 属于 Connection 的成员变量，所以 需要在析构函数中删除 
        Channel* clientChannel_;  // Connection 对应的 的 channel ，在构造函数中创建

        // 添加回调函数
        std::function<void(spConnection)> closeCallBack_;  // TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
        std::function<void(spConnection)> errorCallBack_;  //TCP 连接错误的 回调函数， 提供 Channel 回调
        std::function<void(spConnection, std::string&)> onMessageCallBack_;  // TCP 连接 接收到消息 的 回调函数
        std::function<void(spConnection)> sendCompletionCallback_;   // 发送数据 完成后的回调函数，将回调 void TcpServer::sendCompletionCallback(spConnection connection)

        Buffer inputBuffer_;  // 接收数据的缓冲区
        Buffer outputBuffer_; // 发送数据的缓冲区

        //
        std::atomic_bool disConnect_;  // 客户端 连接是否已经断开，如果断开，则设为 true； 在 IO 线程中会改变这个值，在工作线程中会 读取 判断这个值 ，所以需要使用原子变量

    public:
        Connection(EventLoop* loop, Socket* clientScoket);
        ~Connection();


        int fd() const;
        std::string ip() const;
        uint16_t port() const;




        // 

        void closeCallBack();   // TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
        void errorCallBack();  //TCP 连接错误的 回调函数， 提供 Channel 回调

        void writeCallBack();  // 处理写事件的 回调函数， 供 channel 回调

        // 设置 回调函数
        void setCloseCallBack(std::function<void(spConnection)> closeCallBack);
        void setErrorCallBack(std::function<void(spConnection)> errorCallBack);
        void setOnMessageCallBack(std::function<void(spConnection, std::string&)> onMessageCallBack);
        void setSendCompletionCallback(std::function<void(spConnection)> sendCompletionCallback);

        // 处理 对端 发送过来的消息
        void onMessage();

        // 发送数据
        void send(const char *data,size_t size);        // 发送数据。
};


