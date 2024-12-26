#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include <string>

class InetAddress
{
    private:
        sockaddr_in addr_;  // 表示地址协议的结构体
    public:
        InetAddress();                                          // 构造函数
        InetAddress(const std::string &ip, uint16_t port);       // 构造函数 这里输入 
        ~InetAddress();                                         // 析构函数


        const char *ip() const;  // 获取ip地址
        uint16_t port() const;  // 获取端口号
        const sockaddr *addr() const;
        void setaddr (const sockaddr_in clientaddr);
};

