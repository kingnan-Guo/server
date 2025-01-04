#include "TcpServer.h"


TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadNum): 
threadNum_(threadNum), mainLoop_(new EventLoop()), acceptor_(mainLoop_, ip, port), threadPool_(threadNum_, "IO")
{

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


    // // 创建一个 EventLoop 对象
    // mainLoop_ = new EventLoop();



    // 设置 超时 回调函数
    mainLoop_->setEpollTimeOutCallBack(
        std::bind(&TcpServer::epollTimeOut, this, std::placeholders::_1)
    );



    // acceptor_ = new Acceptor(mainLoop_, ip, port);
    // acceptor_ = new Acceptor(mainLoop_, ip, port); // mainLoop_.get() 获取普通指针

    // 设置 回调函数
    acceptor_.setNewConnectionCallback(
        // std::placeholders::_1 
        // 回调函数，当有新的客户端连接时，此函数会被调用。
        // @param &TcpServer::newConnection       TcpServer对象中的成员函数。
        // @param this                             TcpServer对象。
        // @param std::placeholders::_1            当前无法传输这个值，所以 站位      
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1)
    );



    // // 创建线程池
    // threadPool_ = new ThreadPool(threadNum_, "IO");
    
    // 创建从事件 循环
    for(int i = 0; i < threadNum_; i++){
        // 创建一个 EventLoop 对象,放入到 subLoop_ 容器 中
        // subLoop_.push_back(new EventLoop());
        subLoop_.emplace_back(new EventLoop());




        subLoop_[i]->setEpollTimeOutCallBack(
            std::bind(&TcpServer::epollTimeOut, this, std::placeholders::_1)
        );
        // 把 EventLoop 的 run（） 作为任务 添加给 线程池， 线程池中的线程运行这个任务， 就可以运行事件循环了
        threadPool_.addTask(
            // std::bind(&EventLoop::run, subLoop_[i])
            std::bind(&EventLoop::run, subLoop_[i].get()) //这里需要偶 普通的指针
        );

    }

    // 


}


TcpServer::~TcpServer(){
    // delete acceptor_;
    // delete mainLoop_;

    // 在析构函数 中 释放所有 connections_ 内的  fd 这段是 AI 写的 
    // for (auto it = connections_.begin(); it != connections_.end(); it++){
    //     delete it->second;
    // }

    
    /*
    // 这个时候 是智能指针 系统会 自动 释放
    // 在析构函数 中 释放所有 connections_ 内的  fd
    for (auto &fd:connections_)
    {
        delete fd.second;
    }
    */

    // // 在析构函数 中 释放所有 subLoop_ 内的  fd
    // for (auto &fd:subLoop_)
    // {
    //     delete fd;
    // }

    
    // delete threadPool_;

};

// 运行时间循环 
void TcpServer::start(){
    mainLoop_->run();
};





// ==========

// 处理 客户端的连接请求
void TcpServer::newConnection(std::unique_ptr<Socket> clinetSocket){

    //Connection* connection = new Connection(mainLoop_, clinetSocket); // 这里new 出的对象没有释放

    // 把 客户端的fd 分配给 线程池 中的线程
    // Connection* connection = new Connection(subLoop_[clinetSocket->fd() % threadNum_], clinetSocket); // 这里new 出的对象没有释放
    spConnection connection(new Connection(subLoop_[clinetSocket->fd() % threadNum_], std::move(clinetSocket) ));


    connection->setCloseCallBack(
        // 回调函数，当客户端断开连接时，此函数会被调用。
        // @param &TcpServer::closeConnection       TcpServer对象中的成员函数。
        // @param this                             TcpServer对象。
        // @param std::placeholders::_1            当前无法传输这个值，所以 站位
        std::bind(&TcpServer::closeConnection, this, std::placeholders::_1)
    );

    
    connection->setErrorCallBack(
        // 回调函数，当客户端连接发生错误时，此函数会被调用。
        std::bind(&TcpServer::errorConnection, this, std::placeholders::_1)
    );


    connection->setOnMessageCallBack(
        // 回调函数，
        std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2)
    );

    connection->setSendCompletionCallback(
        // 回调函数，
        std::bind(&TcpServer::sendCompletionCallback, this, std::placeholders::_1)
    );


    printf ("accept client( fd=%d, ip=%s, port=%d ) ok.\n", connection->fd(), connection->ip().c_str(), connection->port());


    // 创建 connection 对象后 把 connection 对象 添加到 connections_  类型为 map 的 容器 中
    connections_[connection->fd()] = connection;

    // 回调 newConnectionCallBack_
    if (newConnectionCallBack_){
        newConnectionCallBack_(connection);
    }

}

// 关闭客户端的连接，在 Connection 类中 回调词函数
// void TcpServer::closeConnection(Connection* connection){
void TcpServer::closeConnection(spConnection connection){

    if(closeConnectionCallBack_){
        closeConnectionCallBack_(connection);
    }


    printf("client ( eventfd = %d ) disconnected.\n", connection->fd());
    // close(connection->fd());                // 关闭 客户端  fd ; 
    connections_.erase(connection->fd());       // 在 connections_ 中删除 connection 对象
    // delete connection;

};

// 客户端连接的错误， 在 Connection 回调 此函数
void TcpServer::errorConnection(spConnection connection){
    if(errorConnectionCallBack_){
        errorConnectionCallBack_(connection);
    }


    printf("client ( eventfd = %d ) error.\n", connection->fd());
    // close(connection->fd());
    connections_.erase(connection->fd());       // 在 connections_ 中删除 connection 对象
    // delete connection;
};


// 收到数据后的处理过程， 在 Connection 类中 回调此函数
void TcpServer::onMessage(spConnection connection, std::string& message){
    
    // printf("client ( eventfd = %d ) message: %s\n", connection->fd(), message.c_str());
    /*
    ///// 定制功能  不通用////
    // 在这里，将经过若干步骤的运算。
    message="reply:"+message;
                
    int len=message.size();                   // 计算回应报文的大小。
    std::string tmpbuf((char*)&len,4);  // 把报文头部填充到回应报文中。
    tmpbuf.append(message);             // 把报文内容填充到回应报文中。
                
    //send(connection->fd(),tmpbuf.data(),tmpbuf.size(),0);   // 把临时缓冲区中的数据直接send()出去。
    connection->send(tmpbuf.data(),tmpbuf.size());   // 把临时缓冲区中的数据直接send()出去。
    */


    // 回调 onMessageCallBack_
    if (onMessageCallBack_){
        onMessageCallBack_(connection, message);
    }

}

void TcpServer::sendCompletionCallback(spConnection connection){
    printf("send completion callback.\n");
    if (sendCompleteCallBack_)
    {
        sendCompleteCallBack_(connection);
    }
}


//  epoll_wait() 超时，在 EvebtLoop 类中回调 此函数
void TcpServer::epollTimeOut(EventLoop * loop){
    printf("epoll_wait timeOut .\n");
    // 可以添加需求代码
    if(timeOutCallBack_){
        timeOutCallBack_(loop);
    }
}



void TcpServer::setNewConnectionCallBack(std::function<void(spConnection)> newConnectionCallBack){
    newConnectionCallBack_ = newConnectionCallBack;
};

void TcpServer::setCloseConnectionCallBack(std::function<void(spConnection)> closeConnectionCallBack){
    closeConnectionCallBack_ = closeConnectionCallBack;
};

void TcpServer::setErrorConnectionCallBack(std::function<void(spConnection)> errorConnectionCallBack){
    errorConnectionCallBack_ = errorConnectionCallBack;
};

void TcpServer::setOnMessageCallBack(std::function<void(spConnection,std::string &message)> onMessageCallBack){
    onMessageCallBack_ = onMessageCallBack;
};

void TcpServer::setSendCompleteCallBack(std::function<void(spConnection)> sendCompleteCallBack){
    sendCompleteCallBack_ = sendCompleteCallBack;
};

void TcpServer::setTimeOutCallBack(std::function<void(EventLoop*)> timeOutCallBack){
    timeOutCallBack_ = timeOutCallBack;
};
