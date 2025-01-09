#include "Connection.h"

/**
 * 这里是 处理与 客户端 通信的类 ，对端发送过来数据。传给 onMessage
 * 
 */
// Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock): loop_(loop), clientScoket_(std::move(clientsock)), disConnect_(false), clientChannel_(new Channel(loop_, clientScoket_->fd()))
// Connection::Connection(const std::unique_ptr<EventLoop>& loop, std::unique_ptr<Socket> clientsock): loop_(loop), clientScoket_(std::move(clientsock)), disConnect_(false), clientChannel_(new Channel(loop_, clientScoket_->fd()))
Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock): loop_(loop), clientScoket_(std::move(clientsock)), disConnect_(false), clientChannel_(new Channel(loop_, clientScoket_->fd()))

{
    // // 创建Channel对象，并添加到epoll中。
    // Channel* clientChannel = new Channel(ep_, clientSock->fd());
    // clientChannel_ = new Channel(loop_, clientScoket_->fd());


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
    // delete clientScoket_; //
    // delete clientChannel_;
    printf("Connection对象已析构。\n");
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

    disConnect_ = true;

    // 要从 tcp 循环中删除 channel
    clientChannel_->remove(); // 关闭读、写事件

    // 在 TcpServer 中，关闭连接，并删除 Channel
    if(closeCallBack_){
        // closeCallBack_(this()); // 回调TcpServer::closeconnection()。
        closeCallBack_(shared_from_this()); // 回调TcpServer::closeconnection()。
    }
};

 //TCP 连接错误的 回调函数， 提供 Channel 回调
void Connection::errorCallBack(){
    // printf("3client(eventfd=%d) error.\n", fd());
    // close(fd());            // 关闭客户端的fd。

    // 要从 tcp 循环中删除 channel
    clientChannel_->remove(); // 关闭读、写事件

    if(errorCallBack_){
        // errorCallBack_(this()); // 回调TcpServer::errorconnection()。
        errorCallBack_(shared_from_this()); // 回调TcpServer::errorconnection()。
    }
};





// 设置 回调函数
void Connection::setCloseCallBack(std::function<void(spConnection)> closeCallBack){
    closeCallBack_ = closeCallBack;
};

void Connection::setErrorCallBack(std::function<void(spConnection)> errorCallBack){
    errorCallBack_ = errorCallBack;
};

// 设置 回调函数 处理 message
void Connection::setOnMessageCallBack(std::function<void(spConnection, std::string&)> onMessageCallBack){
    onMessageCallBack_ = onMessageCallBack;
};

void Connection::setSendCompletionCallback(std::function<void(spConnection)> sendCompletionCallback){
     sendCompletionCallback_ = sendCompletionCallback;
}


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

           std::string message;

            while (true)
            {
                // ////////////////////////////////////////////////////////////
                // //// 这段是 定制的 报文头是 4 个字节 ///////////////// 之后要改成 Buffer类
                // // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
                // int len;
                // memcpy(&len, inputBuffer_.data(), 4);     // 从inputbuffer中获取报文头部。
                // // 如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整。
                // if (inputBuffer_.size()<len+4) break;

                // std::string message(inputBuffer_.data() + 4,len);   // 从inputbuffer中获取一个报文。
                // inputBuffer_.erase(0, len + 4);                                 // 从inputbuffer中删除刚才已获取的报文。
                // ////////////////////////////////////////////////////////////
                

                if(inputBuffer_.pickMessage(message) == false){
                    break;
                }
                printf("recv( eventfd = %d ): %s\n", fd(), message.c_str());

                // std::string message = inputBuffer_.data();
                // printf("message (eventfd=%d):%s\n", fd(), message.data());
                
                // onMessageCallBack_(this(), message); // 回调TcpServer::onMessage()。
                
                // 这个是收到 数据后  调用 callBack 给 业务层
                onMessageCallBack_(shared_from_this(), message); // 回调TcpServer::onMessage()。

                // send(
                //     fd(), inputBuffer_.data(), inputBuffer_.size(), 0
                // );

                //这里已经接收到完整的 报文了 ，更新时间
                lastTime_ = TimeStamp::Now();
                std::cout << "lastTime_ = " << lastTime_.toString() << std::endl;

                
            }


            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            /*
            
            printf("2client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
            */




            disConnect_ = true;
            // 调用 回调函数
            closeCallBack();
            break;
        }
    }
};


// 发送数据。
void Connection::send(const char *data, size_t size)        
{

    if(disConnect_ == true){
        printf("客户端已经断开了  send直接返回 \n");
        return;
    }

    // 判断 当前 线程 是否为 事件循环线程 (IO 线程)
    if(loop_ -> isInLoopThread()){
         printf("Connection::send 在事件循环的 线程中\n");
        // 如果 当前线程是 IO 线程，则直接发送数据
        sendInLoop(data, size);
    } else{
        // 如果 当前线程不是 IO 线程， 把发送的 数据 的 操作 交给 IO 线程 去执行
        printf(" 当前线程不是 IO 线程， 把发送的 数据 的 操作 交给 IO 线程 去执行 \n");
    

        // 把这个函数 传给他 ，让 IO 线程 调用
        // queueInLoop 方法的目的是将某个任务（在此处是 sendInLoop 函数的调用）放入事件循环线程（通常是 I/O 线程）的任务队列中。
        loop_->queueInLoop(
            // std::bind   创建了一个新的可调用对象
            // 通过使用 std::bind，你可以在一个线程中准备要在另一个线程中执行的操作。也就是说，sendInLoop 的实际执行会被延迟到 I/O 线程处理它的时候，而不是在工作线程中立刻执行。
            // 参数确实是传递到了 I/O 线程的上下文中
            std::bind(&Connection::sendInLoop, this, data, size)
        );
    
    }

    printf("Connection::send thread is ( 可能 在 工作 进程里 也可能是 IO 线程)= %d\n", syscall(SYS_gettid));


}


// 发送数据; 如果当前线程是 IO 线程，直接调用次函数， 如果是工作线程，将把次函数 传给 IO 线程，由 IO 线程调用
void Connection::sendInLoop(const char *data,size_t size){
    // outputBuffer_.append(data, size);    // 把需要发送的数据保存到Connection的发送缓冲区中。
    outputBuffer_.appendWithSep(data, size);    // 把需要发送的数据保存到Connection的发送缓冲区中。

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
    printf("Connection::writeCallBack thread is (在 IO 进程里) = %d\n", syscall(SYS_gettid));

    if(outputBuffer_.size() == 0){
        // 如果缓存区为空，则关闭写事件
        clientChannel_->disablewriting();
        // 数据发送完成之后调用 回调函数
        // sendCompletionCallback_(this);
        sendCompletionCallback_(shared_from_this());
        
    }
};



// 判断 TCP 连接是否超时 ， 空闲太久 timeOut
bool Connection::timeOut(time_t now, int timeOut){
    return (now - lastTime_.toInt()) > timeOut;
}