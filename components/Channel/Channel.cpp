#include "Channel.h"



Channel::Channel(Epoll* ep, int fd): ep_(ep), fd_(fd){

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
     ep_->updateChannel(this);
    
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