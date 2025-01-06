#include "EventLoop.h"


// 创建定时器
int createTimerFd(int sec){
    int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK );

    // 设置定时器的初始值和周期。
    struct itimerspec timeout;
    // 注释 
    memset(&timeout, 0, sizeof(struct itimerspec));

    // 设置时间
    timeout.it_value.tv_sec = sec; // 5秒后触发
    timeout.it_value.tv_nsec = 0;// 纳秒

    // 开始计时 alarm(5)
    timerfd_settime(timerFd, 0, &timeout, NULL);

    return timerFd; // 返回定时器fd
}



// 构造函数 
// 创建 ep_  = new Epoll
EventLoop::EventLoop(bool isMainLoop, int timerVl, int timerOut):
    ep_(new Epoll), 
    wakeupFd_(eventfd(0, EFD_NONBLOCK)), 
    wakeChannel_(new Channel(this, wakeupFd_)), 
    timerFd_(createTimerFd(timerOut)), 
    timerChannel_(new Channel(this, timerFd_)),
    isMainLoop_(isMainLoop),
    timeVl_(timerVl),
    timeOut_(timerOut)
{

    // 设置 读 回调
    wakeChannel_->setReadCallback(std::bind(&EventLoop::handleWakeUp, this));
    // 激活 读事件
    wakeChannel_->enableReading();


    // 定时器 注册 读事件
    timerChannel_->setReadCallback(std::bind(&EventLoop::handleTimer, this));
    timerChannel_->enableReading();

}

// 析构函数
// 删除 ep_
EventLoop::~EventLoop()
{
    // delete ep_;
    printf("EventLoop  conn 已析构 \n");
}



// 返回epoll对象
// Epoll* EventLoop::ep()
// {
//     return ep_;
// }

Epoll* EventLoop::ep()
{
    return ep_.get();
}


// 循环
void EventLoop::run(){

    printf("EventLoop::run() start : 线程id thread id  = %d \n", syscall(SYS_gettid));

    threadId_ = syscall(SYS_gettid);// 获取 事件循环 所在 线程id

    // 改造前
    // while (true)        // 事件循环。
    // {
    //     std::vector<Channel *> channels; // 存放epoll_wait()返回事件的数组。
    //     //  
    //     channels = ep.loop();
    //     // 遍历容器 
    //     for(auto &ch:channels)
    //     {
    //         ch->handleEvent();
    //     }
    // }

    // 改造后
    while (true)        // 事件循环。
    {

        
        std::vector<Channel *> channels; // 存放epoll_wait()返回事件的数组。
        //  
        channels = ep_->loop(10 * 1000); // 10s 超时


        // 在事件循环中 判断 channels 是否为空, 如果为空，表示 超时
        if(channels.size() == 0){
            // 调用回调函数
            epollTimeOutCallBack_(this);
        }



        // 遍历容器 
        for(auto &ch:channels)
        {


            ch->handleEvent();// 处理epoll_wait()返回的事件。

        }
    }
}






// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void EventLoop::updateChannel(Channel *ch)                        
{
    ep_->updateChannel(ch);
}

// removeChannel
void EventLoop::removeChannel(Channel *ch)                        
{
    ep_->removeChannel(ch);
}


// void setEpollTimeOutCallBack(std::function<void(EventLoop*)> cb)
void EventLoop::setEpollTimeOutCallBack(std::function<void(EventLoop*)> cb){
    epollTimeOutCallBack_ = cb;
}



// 判断当前线程 是否 为事件循环线程
bool EventLoop::isInLoopThread(){
    //  比较 当前线程id 和 事件循环线程id
    return threadId_ == syscall(SYS_gettid);
}

// 把任务 添加到 队列中 
void EventLoop::queueInLoop(std::function<void()> fn){
    {// 锁 ： 给任务队列 加锁
        std::lock_guard<std::mutex> gd(mutex_);
        // 把 函数 对象 是不是
        taskQueue_.push(fn); // 把任务 添加到 队列中
    }

    // 唤醒 事件循环
    wakeup();
}



// 唤醒 事件循环的函数， 用 eventfd 唤醒事件 循环线程
// 唤醒 方法 是 往 eventfd 中 写入一点东西 
void EventLoop::wakeup(){
    uint64_t val = 1;
    write(wakeupFd_, &val, sizeof(val));
};


// 增加一个 事件 循环 线程 被eventfd 唤醒 ， 被唤醒后 执行的 函数
void EventLoop::handleWakeUp(){
    printf("EventLoop::handleWakeUp()    线程id thread id  = %d \n", syscall(SYS_gettid));

    // 把 eventfd 中的数据 读出来
    uint64_t val = 0;
    // 必须要读取 出来， 如果 不读取 在水平出发的时候 会 导致 eventfd 一直 触发
    read(wakeupFd_, &val, sizeof(val));


    std::function<void()> fn;
    std::lock_guard<std::mutex> gd(mutex_); // 锁 ： 给任务队列 加锁


    // 如果 队列 不为 空， 出队 一个任务
    while(taskQueue_.size() > 0){

        fn = std::move(taskQueue_.front());// 移动语义
        taskQueue_.pop(); // 出队
        fn(); // 执行任务

    }

};



// 定时器相关： 闹钟响时 执行的函数
void EventLoop::handleTimer(){
    // 重新计时

    // 设置定时器的初始值和周期。
    struct itimerspec timeout;
    // 注释 
    memset(&timeout, 0, sizeof(struct itimerspec));

    // 设置时间
    timeout.it_value.tv_sec = timeVl_; // 5秒后触发
    timeout.it_value.tv_nsec = 0;// 纳秒

    // 开始计时 alarm(5)
    timerfd_settime(timerFd_, 0, &timeout, NULL);


    if(isMainLoop_){
         printf("alarm  time out  handleTimer() 主  线程id thread id  = %d \n", syscall(SYS_gettid));
    } else {
        //  闹钟时间到了， 遍历 ConnectionMap_ ，判断每个 connection 对象是否超时
        printf("alarm  time out  handleTimer() 从 线程id thread id  = %d  fd = ", syscall(SYS_gettid));
    
        time_t now = time(NULL);
        for(auto &it:ConnectionMap_){
            printf("%d ", it.first);
            if(it.second->timeOut(now, timeOut_)){
                {
                    std::lock_guard<std::mutex> gd(connectionsMapMutex_);// 加锁
                    // 超时 从容器中删除 connection
                    ConnectionMap_.erase(it.first);// 也可以 ConnectionMap_.erase(it.second->fd());
                }

                if(timerCallBack_){
                    timerCallBack_(it.first);
                }
            }
        }
        printf("\n");
    
    }



};


//添加 Connection 到  ConnectionMap_ 中的函数
void EventLoop::newConnection(spConnection connection){
    std::lock_guard<std::mutex> gd(connectionsMapMutex_);// 加锁
    ConnectionMap_[connection->fd()] = connection;
};




// 设置 回调函数的 成员函数 // 将被设置为TcpServer::removeConnectionCallBack()
void EventLoop::setTimerCallBack(std::function<void(int)> cb){
    timerCallBack_ = cb;
};

