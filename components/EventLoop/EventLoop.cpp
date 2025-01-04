#include "EventLoop.h"
#include <sys/syscall.h>
#include <unistd.h>

// 构造函数 
// 创建 ep_  = new Epoll
EventLoop::EventLoop():ep_(new Epoll)
{

}

// 析构函数
// 删除 ep_
EventLoop::~EventLoop()
{
    delete ep_;
}



// 返回epoll对象
Epoll* EventLoop::ep()
{
    return ep_;
}


// 循环
void EventLoop::run(){

    printf("EventLoop::run() start : 线程id thread id  = %d \n", syscall(SYS_gettid));

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