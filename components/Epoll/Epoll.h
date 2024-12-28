#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>

class Epoll
{
    private:
        static const int MAX_EVENTS = 100;   // epoll_wait() 一次能处理的最大事件数
        int epoll_fd_ = -1;                 // epoll 句柄 在构造函数中创建
        epoll_event events_[MAX_EVENTS];    // 存放epoll_wait()返回事件的数组。
    public:
        Epoll();
        ~Epoll();


        void addFd(int epollfd, uint32_t opt); // 添加文件描述符到epoll中

        // 在这个函数中增加 成员函数 ，在这个函数中 运行 epoll_wait， 等待事件的发生, 已经发生的事件 用 vector 返回

        std::vector<epoll_event> loop(int timeout = -1);
};
