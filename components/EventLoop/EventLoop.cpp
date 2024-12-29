#include "EventLoop.h"


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
        channels = ep_->loop();



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