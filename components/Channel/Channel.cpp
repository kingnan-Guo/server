#include "Channel.h"



// Channel::Channel(Epoll* ep, int fd, bool isListen): ep_(ep), fd_(fd), isListen_(isListen){

// }
// Channel::Channel(Epoll* ep, int fd): ep_(ep), fd_(fd){

// }

Channel::Channel(EventLoop* loop, int fd): loop_(loop), fd_(fd)
// Channel::Channel(const std::unique_ptr<EventLoop>& loop, int fd): loop_(loop), fd_(fd)
{

}

// 析构函数
// 在 析构函数 中 ， 不要销毁 ep_ ,也不能关闭 fd_, 因为这两个东西不属于 Channek 类， Channel 类置使需要用这两个东西，但是不拥有
Channel::~Channel(){
    
}

// 返回 fd
int Channel::fd(){
    return fd_;
}; 

 // 采用边缘 触发
void Channel::useEt(){
    events_ |= EPOLLET; // 设置边缘触发

};



// 让 epoll_wait() 监视 fd 的 读事件
void Channel::enableReading(){
    // 要监听 读事件
    events_ =  events_ | EPOLLIN;
    // 调用 Epoll 类中的  updateChannel 。epoll_ctl() 将 fd 加入到 epoll 中
    //  ep_->updateChannel(this);
    loop_->updateChannel(this);
    
};

// 让 epoll_wait() 停止监视 fd 的 读事件
void Channel::disableReading(){
    // 要监听 读事件
    events_ =  events_ |  ~EPOLLIN;
    loop_->updateChannel(this);
};

// 取消所有事件
void Channel::disableAll(){
    events_ = 0;
    loop_->updateChannel(this);
};                          // 取消所有事件
// 从 事件循环中删除 channel ， 也就是 从 epoll 中删除 channel？？
void Channel::remove(){
    disableAll();               // 取消所有事件 可有可无
    loop_->removeChannel(this); // 从红黑树上删除 fd
}; 



// 注册写事件。
void Channel::enablewriting(){
    events_|=EPOLLOUT;
    loop_->updateChannel(this);
};

// 取消写事件。  
void Channel::disablewriting(){
    events_&=~EPOLLOUT;
    loop_->updateChannel(this);
};                     




// 设置 channel 是否在 epoll 中; 默认 把成员变量设置为 true
void Channel::setInEpoll(bool inEpoll){
    inEpoll_ = inEpoll;
}; 

// 返回 channel 是否在 epoll 中
bool Channel::inEpoll(){
    return inEpoll_;
};

// 返回 events_
uint32_t Channel::events(){
    return events_;
};

 // 设置 revents_ 为 revents
void Channel::setRevents(uint32_t revents){
    revents_ = revents;
};

// 返回 revents_
uint32_t Channel::revents(){
    return revents_;
}; 


// ========================================================================================
// 事件处理函数 ， epoll_wait() 返回的时候， 执行它
void Channel::handleEvent(){

    {
        // 如果是客户端连接的fd有事件。
        ////////////////////////////////////////////////////////////////////////
        if (revents_ & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
        {
            /*
            printf("1client(eventfd=%d) disconnected.\n", fd_);
            close(fd_);            // 关闭客户端的fd。
            
            */

           printf("handleEvent EPOLLRDHUP \n");
           closeCallBack_();






        } //  普通数据  带外数据

        else if (revents_ & (EPOLLIN | EPOLLPRI))   // 接收缓冲区中有数据可以读。
        {

            printf("handleEvent  EPOLLIN | EPOLLPRI \n");
            readCallback_();

            
        }

        else if (revents_ & EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
        {
            printf("handleEvent EPOLLOUT \n");
            writeCallback_(); // 调 用回调函数 ， connection 类中定义的回调函数

        }

        else // 其它事件，都视为错误。
        {
            /*
            printf("3client(eventfd=%d) error.\n", fd_);
            close(fd_);            // 关闭客户端的fd。
            
            */

            printf("handleEvent OTHERS \n");

            errorCallBack_();



        }
        ////////////////////////////////////////////////////////////////////////

    }




};


//==========================    =========================================
/*

// 此处 修改到 Connection 类中

#include "Connection.h"
// 将 客户端连上来 和  连接的客户端的fd有事件 封装成 回调函数
// 处理新客户端连接请求
void Channel::newConnection(Socket * serverScoket){
    InetAddress clientaddr;    
    Socket *clientSock = new Socket(serverScoket->accept(clientaddr));

    printf ("accept client(fd=%d, ip=%s, port=%d ) ok.\n", clientSock->fd(), clientaddr.ip(), clientaddr.port());

    //////////////////////////////////////////////////////////////////////
    // // // 创建Channel对象，并添加到epoll中。
    // // Channel* clientChannel = new Channel(ep_, clientSock->fd());
    // Channel* clientChannel = new Channel(loop_, clientSock->fd());


    // clientChannel->setReadCallback(
    //     std::bind(&Channel::onMessage, clientChannel)
    // );

    // // 客户端采用边缘触发
    // clientChannel->useEt();
    // clientChannel->enableReading();
    //////////////////////////////////////////////////////////////////////


   // 创建Connection对象，并添加到epoll中。
   Connection* connection = new Connection(loop_, clientSock);

}; 
*/





// 废弃 ：  处理 对端 发送过来的消息
void Channel::onMessage(){
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n", fd_, buffer);
            send(fd_, buffer, strlen(buffer), 0);
        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            /*
            
            printf("2client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
            */

           // 调用 回调函数
           closeCallBack_();
            break;
        }
    }
};





 // 设置  回调函数的成员函数。readCallback_ 
void Channel::setReadCallback(std::function<void()> readCallback)
{
    readCallback_ = readCallback;
}




// 设置 关闭 fd_ 的 回调函数； 将 回调 Connection::closeCallBack()
void Channel::setCloseCallBack(std::function<void()> closeCallBack){
    closeCallBack_ = closeCallBack;
};
 // 设置 fd_ 错误事件的。回调函数 ;  fd_ 发生错误 的 回调函数； 将 回调 Connection::errorCallBack()  
void Channel::setErrorCallBack(std::function<void()> errorCallBack){
    errorCallBack_ = errorCallBack;
};


void Channel::setWriteCallBack(std::function<void()> writeCallBack){
    writeCallback_ = writeCallBack;
};








