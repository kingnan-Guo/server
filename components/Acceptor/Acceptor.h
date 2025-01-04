#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include <memory>// 使用只能指针 
#include <atomic>

class Acceptor
{
    private:
        EventLoop* loop_;    // Acceptor 对应的事件循环， 在构造函数中传入, 从外面传过来的 
        Socket serverScoket_;  // 成员变量 ；服务端用于监听socket ，在构造函数中 创建
        Channel acceptChannel_;  //  Acceptor 对应的 的 channel ，在构造函数中创建


        // 使用 回调函数
        std::function<void(std::unique_ptr<Socket>)> newConnectionCallback_;  // 处理新客户端的连接请求的  回调函数，指向  TcpServer::newConnection

    public:
        Acceptor(EventLoop* loop, const std::string &ip, uint16_t port);
        ~Acceptor();

        // 这里是将 Channel 里的 newConnection 放到这里。 
        // 将 客户端连上来 和  连接的客户端的fd有事件 封装成 回调函数 ===== 
        void newConnection(); // 处理新客户端连接请求


        // 增加一个 成员函数 用于 回调函数
        void setNewConnectionCallback(std::function<void(std::unique_ptr<Socket>)> callback); // 设置处理新客户端 连接请求的。回调函数， 回调函数的参数是 Socket* ， 也就是 新连接的客户端的 fd

};




// 在传统的网络服务 链接中 由 监听的 fd 产生 客户端 连接的 fd ； Acceptor 封装了监听的fd 监听的 Channel， Connection类的对象由 Acceptor 类 创建， 也就是说 创建。Connection的代码应该 写在 Acceptor 类中
