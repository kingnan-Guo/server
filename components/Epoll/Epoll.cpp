
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


// updateChannel 把 channel 添加 或 更新到 红黑树上， Channel 中有哦 fd ， 也需要监视的事件
void Epoll::updateChannel(Channel* ch){
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();


    // 如果已经添加过，则更新。
    if(ch->inEpoll()){
        // EPOLL_CTL_MOD 修改监听事件
        if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1){
            printf("epoll_ctl update fail\n");
            exit(1);
        }
    }
    // 如果没有添加过，则添加。
    else {

        // 把需要监视的listenfd和它的事件加入epollfd中。
        if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1){
            printf("epoll_ctl add fail\n");
            exit(1);
        }

        // 添加之后 给 ch 的 inEpoll_ 赋值为 true
        ch->setInEpoll(true);

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

/**
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
 */


std::vector<Channel*>  Epoll::loop(int timeout){
    //先申请一个容器
    std::vector<Channel * > channels;

    bzero(events_, sizeof(events_)); // 清空events_数组

    int infds = epoll_wait(epoll_fd_, events_, MAX_EVENTS, timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        // EBADF ：epfd不是一个有效的描述符。
        // EFAULT ：参数events指向的内存区域不可写。
        // EINVAL ：epfd不是一个epoll文件描述符，或者参数maxevents小于等于0。
        // EINTR ：阻塞过程中被信号中断，epoll_pwait()可以避免，或者错误处理中，解析error后重新调用epoll_wait()。
        // 在Reactor模型中，不建议使用信号，因为信号处理起来很麻烦，没有必要。------ 陈硕
        perror("epoll_wait() failed"); 
        exit(1);
    }

    // 超时。
    if (infds == 0)
    {
        // 如果 epoll_wait() 超时，表示系统空闲， 返回的 channels 容器为空
        // printf("epoll_wait() timeout.\n"); 
        return channels; // 如果超时。返回 空的容器
    }

    // 如果infds>0，表示有事件发生的fd的数量。
    for (int ii = 0; ii < infds; ii++)       // 遍历epoll返回的数组evs。
    {
        // evs.push_back(events_[ii]);// 将发生事件的fd放入容器中

        // 将 已发生事件的 channel   prt 取出来， 放入 channels 中
        Channel* ch = ( Channel* )events_[ii].data.ptr;
        // 设置 channel 中的 revents 成员 ，为什么设置 已经发生的事件 ？？ 
        ch -> setRevents(events_[ii].events);

        // 最后把 channel 放入 channels 容器 中
        channels.push_back(ch);
    }
    // 返回容器不会造成大量数据的 拷贝
    return channels;
}




