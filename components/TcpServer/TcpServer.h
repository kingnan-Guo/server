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

#include "ThreadPool.h"


// 网络服务类 
class TcpServer{
    private:
        EventLoop* mainLoop_; // 主事件循环；堆内存。 之前 不使用置针的时候 是 栈内存    一个 Tcp Server可以有多个事件循环，现在是单线程，暂时只用一个事件循环
        
        std::vector<EventLoop*> subLoop_; // 存放 子事件循环 的容器 ；堆内存
        
        
        Acceptor* acceptor_; // 一个 Tcp Server 只有 一个  Acceptor 对象

        ThreadPool* threadPool_; // 一个 Tcp Server 有一个线程池
        int threadNum_; //线程池大小， 即 从事件循环的个数， 线程池中线程的数量




        // 使用 map 来管理所有的连接
        std::map<int, spConnection> connections_; // int 整数  ； Connection* 指针 ； 一个 TcpServer 有多个 Connection 对象， 存放在 map 容器中

        std::function<void(spConnection)> newConnectionCallBack_;                // 回调EchoServer::HandleNewConnection()。
        std::function<void(spConnection)> closeConnectionCallBack_;              // 回调EchoServer::HandleClose()。
        std::function<void(spConnection)> errorConnectionCallBack_;              // 回调EchoServer::HandleError()。
        std::function<void(spConnection,std::string &message)> onMessageCallBack_;        // 回调EchoServer::HandleMessage()。
        std::function<void(spConnection)> sendCompleteCallBack_;                 // 回调EchoServer::HandleSendComplete()。
        std::function<void(EventLoop*)>  timeOutCallBack_;                       // 回调EchoServer::HandleTimeOut()。


    public:
        // 需要把 ip 和 端口 传进来
        TcpServer(const std::string &ip, const uint16_t port, int threadNum = 0);
        ~TcpServer();

        // 运行时间循环
        void start();
        void newConnection(Socket* clinetSocket);


        void closeConnection(spConnection connection); // 关闭客户端的连接，在 Connection 类中 回调词函数
        void errorConnection(spConnection connection); // 客户端连接的错误， 在 Connection 回调 此函数
        
        // 收到数据后的处理过程， 在 Connection 类中 回调此函数
        void onMessage(spConnection connection, std::string & message);

        // 发送数据完成后的处理过程， 在 Connection 类中 回调此函数
        void sendCompletionCallback(spConnection connection);
        // epoll_wait() 超时，在 EvebtLoop 类中回调 此函数
        void epollTimeOut(EventLoop * loop);// 现在的 tcp serve 中只有一个事件循环，在以后的 版本中，可以扩展为多个事件循环， 所以要传 loop 参数



        // 设置回调函数
        void setNewConnectionCallBack(std::function<void(spConnection)> newConnectionCallBack);
        void setCloseConnectionCallBack(std::function<void(spConnection)> closeConnectionCallBack);
        void setErrorConnectionCallBack(std::function<void(spConnection)> errorConnectionCallBack);
        void setOnMessageCallBack(std::function<void(spConnection,std::string & message)> onMessageCallBack);
        void setSendCompleteCallBack(std::function<void(spConnection)> sendCompleteCallBack);
        void setTimeOutCallBack(std::function<void(EventLoop*)> timeOutCallBack);

        
};