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


    // clientChannel_->setReadCallback(
    //     std::bind(&Channel::onMessage, clientChannel_)
    // );
    clientChannel_->setReadCallback(
        std::bind(&Connection::onMessage, this)
    );

    // 设置回调函数
    clientChannel_->setCloseCallBack(
        std::bind(&Connection::closeCallBack, this)
    );

    clientChannel_->setErrorCallBack(
        std::bind(&Connection::errorCallBack, this)
    );


    clientChannel_->setWriteCallBack(
        std::bind(&Connection::writeCallBack, this)
    );


    // 客户端采用边缘触发
    clientChannel_->useEt(); // 暂时 注掉 变成水平触发， 这样 EPOLLOUT 会触发很多次
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
    // printf("1client(eventfd=%d) disconnected.\n", fd());
    // close(fd());     

    // 在 TcpServer 中，关闭连接，并删除 Channel
    if(closeCallBack_){
        closeCallBack_(this); // 回调TcpServer::closeconnection()。
    }
};

 //TCP 连接错误的 回调函数， 提供 Channel 回调
void Connection::errorCallBack(){
    // printf("3client(eventfd=%d) error.\n", fd());
    // close(fd());            // 关闭客户端的fd。
    if(errorCallBack_){
        errorCallBack_(this); // 回调TcpServer::errorconnection()。
    }
};





// 设置 回调函数
void Connection::setCloseCallBack(std::function<void(Connection*)> closeCallBack){
    closeCallBack_ = closeCallBack;
};

void Connection::setErrorCallBack(std::function<void(Connection*)> errorCallBack){
    errorCallBack_ = errorCallBack;
};

// 设置 回调函数 处理 message
void Connection::setOnMessageCallBack(std::function<void(Connection*, std::string)> onMessageCallBack){
    onMessageCallBack_ = onMessageCallBack;
};


//   处理 对端 发送过来的消息
void Connection::onMessage(){
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n", fd(), buffer);
            // send(fd(), buffer, strlen(buffer), 0);


            // 把数据放到接收 缓存区
            inputBuffer_.append(buffer, nread);

        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {

            /*

            outputBuffer_ = inputBuffer_; // 把接收到的数据，放到发送缓存区
            inputBuffer_.clear(); // 清空接收缓存区

            // 把发送数据缓存区的 数据直接 send 出去
            send(
                fd(), outputBuffer_.data(), outputBuffer_.size(), 0
            );// 发送数据

            */

            while (true)
            {
                ////////////////////////////////////////////////////////////
                //// 这段是 定制的 报文头是 4 个字节 ///////////////// 之后要改成 Buffer类
                // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
                int len;
                memcpy(&len, inputBuffer_.data(), 4);     // 从inputbuffer中获取报文头部。
                // 如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整。
                if (inputBuffer_.size()<len+4) break;

                std::string message(inputBuffer_.data() + 4,len);   // 从inputbuffer中获取一个报文。
                inputBuffer_.erase(0, len + 4);                                 // 从inputbuffer中删除刚才已获取的报文。
                ////////////////////////////////////////////////////////////
                
                printf("recv( eventfd = %d ): %s\n", fd(), message.c_str());

                // std::string message = inputBuffer_.data();
                // printf("message (eventfd=%d):%s\n", fd(), message.data());
                onMessageCallBack_(this, message); // 回调TcpServer::onMessage()。

                // send(
                //     fd(), inputBuffer_.data(), inputBuffer_.size(), 0
                // );
            }


            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            /*
            
            printf("2client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
            */

            // 调用 回调函数
            closeCallBack();
            break;
        }
    }
};


// 发送数据。
void Connection::send(const char *data,size_t size)        
{
    outputBuffer_.append(data, size);    // 把需要发送的数据保存到Connection的发送缓冲区中。

    // 注册 写事件， 如果 数据缓存区 可以写入 ，那么就立即 发送; 
    clientChannel_->enablewriting();
}



// 处理写事件的 回调函数， 供 channel 回调
void Connection::writeCallBack(){
    printf("writeCallBack 333\n");
    // 把缓存区中的数据发送出去
    int writen = ::send(fd(), outputBuffer_.data(), outputBuffer_.size(), 0);
    if(writen > 0){
        outputBuffer_.erase(0, writen); // 从缓存区中删除已发送的数据。

    }

    if(outputBuffer_.size() == 0){
        // 如果缓存区为空，则关闭写事件
        clientChannel_->disablewriting();
    }
};  