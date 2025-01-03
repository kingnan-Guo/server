#pragma once
#include <sys/epoll.h>
#include <map>
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"

#include "Connection.h"


// 网络服务类 
class TcpServer{
    private:
        EventLoop loop_; // 一个 Tcp Server可以有多个事件循环，现在是单线程，暂时只用一个事件循环
        Acceptor* acceptor_; // 一个 Tcp Server 只有 一个  Acceptor 对象

        // 使用 map 来管理所有的连接
        std::map<int, Connection*> connections_; // int 整数  ； Connection* 指针 ； 一个 TcpServer 有多个 Connection 对象， 存放在 map 容器中

        std::function<void(Connection*)> newConnectionCallBack_;                // 回调EchoServer::HandleNewConnection()。
        std::function<void(Connection*)> closeConnectionCallBack_;              // 回调EchoServer::HandleClose()。
        std::function<void(Connection*)> errorConnectionCallBack_;              // 回调EchoServer::HandleError()。
        std::function<void(Connection*,std::string &message)> onMessageCallBack_;        // 回调EchoServer::HandleMessage()。
        std::function<void(Connection*)> sendCompleteCallBack_;                 // 回调EchoServer::HandleSendComplete()。
        std::function<void(EventLoop*)>  timeOutCallBack_;                       // 回调EchoServer::HandleTimeOut()。


    public:
        // 需要把 ip 和 端口 传进来
        TcpServer(const std::string &ip, const uint16_t port);
        ~TcpServer();

        // 运行时间循环
        void start();
        void newConnection(Socket* clinetSocket);


        void closeConnection(Connection* connection); // 关闭客户端的连接，在 Connection 类中 回调词函数
        void errorConnection(Connection* connection); // 客户端连接的错误， 在 Connection 回调 此函数
        
        // 收到数据后的处理过程， 在 Connection 类中 回调此函数
        void onMessage(Connection* connection, std::string message);

        // 发送数据完成后的处理过程， 在 Connection 类中 回调此函数
        void sendCompletionCallback(Connection* connection);
        // epoll_wait() 超时，在 EvebtLoop 类中回调 此函数
        void epollTimeOut(EventLoop * loop);// 现在的 tcp serve 中只有一个事件循环，在以后的 版本中，可以扩展为多个事件循环， 所以要传 loop 参数



        // 设置回调函数
        void setNewConnectionCallBack(std::function<void(Connection*)> newConnectionCallBack);
        void setCloseConnectionCallBack(std::function<void(Connection*)> closeConnectionCallBack);
        void setErrorConnectionCallBack(std::function<void(Connection*)> errorConnectionCallBack);
        void setOnMessageCallBack(std::function<void(Connection*,std::string &message)> onMessageCallBack);
        void setSendCompleteCallBack(std::function<void(Connection*)> sendCompleteCallBack);
        void setTimeOutCallBack(std::function<void(EventLoop*)> timeOutCallBack);

        
};