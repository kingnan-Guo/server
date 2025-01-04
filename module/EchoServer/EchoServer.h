#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

#include "ThreadPool.h"

class EchoServer
{
    private:
        TcpServer tcpServer_;
        ThreadPool threadPool_;// 工作线程池 
    public:
        EchoServer(const std::string &ip, const uint16_t port, int subThreadNum = 2, int workThreadNum = 3);
        ~EchoServer();

        void Start();          // 启动服务器


        void HandleNewConnection(Connection* connection);               // 处理新客户端连接请求，在TcpServer类中回调此函数。
        void HandleClose(Connection *connection);                       // 关闭客户端的连接，在TcpServer类中回调此函数。 
        void HandleError(Connection *connection);                       // 客户端的连接错误，在TcpServer类中回调此函数。
        void HandleMessage(Connection *connection, std::string& message);     // 处理客户端的请求报文，在TcpServer类中回调此函数。
        void HandleSendComplete(Connection *connection);                // 数据发送完成后，在TcpServer类中回调此函数。
        void HandleTimeOut(EventLoop *loop);                            // epoll_wait()超时，在TcpServer类中回调此函数。

        // 
        void OnMessage(Connection *connection, std::string& message);     // 处理客户端的请求报文, 用于添加给线程池,在ThreadPool中回调此函数。
};


