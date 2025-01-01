#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, const uint16_t port){

    /*
    // Socket serverScoket(createnonblocking()); 不能使用这个 ，因为 离开 TcpServer 后 会调用  ~Socket() ，会关闭fd，导致epoll监听失败。 所以使用下面的 
    Socket * serverScoket = new Socket(createnonblocking()); // 

    InetAddress servaddr(ip, port);

    serverScoket->setreuseaddr(true);
    serverScoket->settcpnodelay(true);
    serverScoket->setreuseport(true);
    serverScoket->setkeepalive(true);

    serverScoket->bind(servaddr);
    serverScoket->listen();

    //---------------


    // 创建服务端用于监听的listenfd。
    int listenfd = serverScoket->fd();


    // // 创建epoll句柄（红黑树）。
    // Epoll ep;
    
    // // 让epoll监视listenfd的读事件，采用水平触发。
    // Channel* servChannel = new Channel(&ep, listenfd);

    // EventLoop eLoop;
    // Channel* servChannel = new Channel(eLoop.ep(), listenfd);// 这里new出来的对象没有释放，这个问题以后再解决。
    Channel* servChannel = new Channel(&loop_, listenfd);


    // 指定回调函数 
    servChannel->setReadCallback(
        // 
        // 回调函数，当有新的客户端连接时，此函数会被调用。
        // @param &Channel::newConnection       Channel对象中的成员函数。
        // @param servChannel                   服务端用于监听的Channel对象。
        //  @param &serverScoket                newConnection 的传值参数。
        // 
        // 
        // 
        std::bind(&Channel::newConnection, servChannel, serverScoket) // 绑定函数
    );

    // 启用 读事件
    servChannel->enableReading();

    */

   acceptor_ = new Acceptor(&loop_, ip, port);

    // 设置 回调函数
    acceptor_->setNewConnectionCallback(
        // std::placeholders::_1 
        // 回调函数，当有新的客户端连接时，此函数会被调用。
        // @param &TcpServer::newConnection       TcpServer对象中的成员函数。
        // @param this                             TcpServer对象。
        // @param std::placeholders::_1            当前无法传输这个值，所以 站位      
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1)
    );


};


TcpServer::~TcpServer(){
    delete acceptor_;

    // 在析构函数 中 释放所有 connections_ 内的  fd 这段是 AI 写的 
    // for (auto it = connections_.begin(); it != connections_.end(); it++){
    //     delete it->second;
    // }
    // 在析构函数 中 释放所有 connections_ 内的  fd
    for (auto &fd:connections_)
    {
        delete fd.second;
    }
    

};

// 运行时间循环 
void TcpServer::start(){
    loop_.run();
};





// ==========

// 处理 客户端的连接请求
void TcpServer::newConnection(Socket* clinetSocket){

    Connection* connection = new Connection(&loop_, clinetSocket); // 这里new 出的对象没有释放

    printf ("accept client( fd=%d, ip=%s, port=%d ) ok.\n", connection->fd(), connection->ip().c_str(), connection->port());


    // 创建 connection 对象后 把 connection 对象 添加到 connections_  类型为 map 的 容器 中
    connections_[connection->fd()] = connection;

}

