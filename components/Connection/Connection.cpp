#include "Connection.h"

/**
 * 这里是 处理与 客户端 通信的类 ，对端发送过来数据。传给 onMessage
 * 
 */
Connection::Connection(EventLoop* loop, Socket* clientsock): loop_(loop), clientScoket_(clientsock)
{
    // // 创建Channel对象，并添加到epoll中。
    // Channel* clientChannel = new Channel(ep_, clientSock->fd());
    clientChannel_ = new Channel(loop_, clientScoket_->fd());


    clientChannel_->setReadCallback(
        std::bind(&Channel::onMessage, clientChannel_)
    );

    // 客户端采用边缘触发
    clientChannel_->useEt();
    clientChannel_->enableReading();
}


// 
Connection::~Connection()
{
    delete clientScoket_; //
    delete clientChannel_;
}




int Connection::fd() const{
    return clientScoket_->fd();
};

std::string Connection::ip() const{
    return clientScoket_->ip();
};

uint16_t Connection::port() const{
    return clientScoket_->port();
};