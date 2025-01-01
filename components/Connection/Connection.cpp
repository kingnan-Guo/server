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


    // 设置回调函数
    clientChannel_->setCloseCallBack(
        std::bind(&Connection::closeCallBack, this)
    );

    clientChannel_->setErrorCallBack(
        std::bind(&Connection::errorCallBack, this)
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


// TCP 连接 关闭 断开 的 回调函数， 供 Channel 回调
void Connection::closeCallBack(){
    printf("1client(eventfd=%d) disconnected.\n", fd());
    close(fd());     
};

 //TCP 连接错误的 回调函数， 提供 Channel 回调
void Connection::errorCallBack(){
    printf("3client(eventfd=%d) error.\n", fd());
    close(fd());            // 关闭客户端的fd。
}; 

