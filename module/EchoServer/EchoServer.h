#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

#include "ThreadPool.h"


class UserInfo  // 用户信息（状态机）。
{
    private:
        int fd_;                      // 客户端的fd。
        std::string ip_;           // 客户端的ip地址。
        bool login_=false;    // 客户端登录的状态：true-已登录；false-未登录。
    public:
        UserInfo(int fd,const std::string &ip):fd_(fd),ip_(ip) {}
        void setLogin(bool login) { login_=login; }
        bool Login() { return login_; }
};


class EchoServer
{
    private:
        using spUserInfo = std::shared_ptr<UserInfo>;
        TcpServer tcpServer_;
        ThreadPool threadPool_;// 工作线程池 
        std::mutex userMapMutex_;
        std::map<int, spUserInfo> userMap_; // 用户信息表
    public:
        EchoServer(const std::string &ip, const uint16_t port, int subThreadNum = 2, int workThreadNum = 3);
        ~EchoServer();

        void Start();          // 启动服务器
        void Stop();                // 停止服务。

        void HandleNewConnection(spConnection connection);               // 处理新客户端连接请求，在TcpServer类中回调此函数。
        void HandleClose(spConnection connection);                       // 关闭客户端的连接，在TcpServer类中回调此函数。 
        void HandleError(spConnection connection);                       // 客户端的连接错误，在TcpServer类中回调此函数。
        void HandleMessage(spConnection connection, std::string& message);     // 处理客户端的请求报文，在TcpServer类中回调此函数。
        void HandleSendComplete(spConnection connection);                // 数据发送完成后，在TcpServer类中回调此函数。
        void HandleTimeOut(EventLoop *loop);                            // epoll_wait()超时，在TcpServer类中回调此函数。

        // 
        void OnMessage(spConnection connection, std::string& message);     // 处理客户端的请求报文, 用于添加给线程池,在ThreadPool中回调此函数。
        // 客户端连接超时回调, 客户端连接超时的 时候 TcpServer类中回调此函数
        void HandleRemove(int fd);
};


