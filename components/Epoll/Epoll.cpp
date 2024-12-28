
#include "Epoll.h"



Epoll::Epoll()
{
    epoll_fd_ = epoll_create(1);        // 创建epoll句柄（红黑树）。
    if(epoll_fd_ == -1){
        printf("epoll_create fail\n");
        exit(1);
    }
}

Epoll::~Epoll()
{
    ::close(epoll_fd_);
}



void Epoll::addFd(int fd, uint32_t opt){

    struct epoll_event ev;              // 声明事件的数据结构。
    ev.data.fd = fd;                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = opt;                //opt =  EPOLLIN 让epoll监视listenfd的读事件，采用水平触发。

    // 把需要监视的listenfd和它的事件加入epollfd中。
    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1){
        printf("epoll_ctl fail\n");
        exit(1);
    }
}


/**
 * 
 * 在这个函数中增加 成员函数 ，在这个函数中 运行 epoll_wait， 等待事件的发生, 已经发生的事件 用 vector 返回
 * 步骤：
 * 1. 先申请一个容器
 * 2. 清空 events_数组
 * 3. 调用 epoll_wait
 * 4. 将发生事件的fd放入容器中
 * 
 */
std::vector<epoll_event> Epoll::loop(int timeout){
    //先申请一个容器
    std::vector<epoll_event> evs;

    bzero(events_, sizeof(events_)); // 清空events_数组

    int infds = epoll_wait(epoll_fd_, events_, MAX_EVENTS, timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        perror("epoll_wait() failed"); 
        exit(1);
    }

    // 超时。
    if (infds == 0)
    {
        printf("epoll_wait() timeout.\n"); 
        return evs;
    }

    // 如果infds>0，表示有事件发生的fd的数量。
    for (int ii = 0; ii < infds; ii++)       // 遍历epoll返回的数组evs。
    {
        evs.push_back(events_[ii]);// 将发生事件的fd放入容器中
    }
    // 返回容器不会造成大量数据的 拷贝
    return evs;
}