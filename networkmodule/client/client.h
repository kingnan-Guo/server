#ifndef __SYLAR_CLIENT_H
#define __SYLAR_CLIENT_H
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
using namespace std;

class ctcpclient{
    private:
        int m_clientfd;                                 // 客户端 socket
        string m_ip;                                    // 客户端 ip
        unsigned short m_port;                          // 客户端端口
    public:
        ctcpclient(): m_clientfd(-1),m_port(0){};                   // m_clientfd(-1) m_clientfd 初始化为-1 ： m_clientfd = -1
        bool connect(const string &in_ip, const unsigned short in_port);  // 连接服务器 向服务端发起连接请求，成功返回true，失败返回false。
        //向服务端发送报文，成功返回true，失败返回false。
        bool send(const string &buffer);                                 //buffer不要用constchar*

        bool send( void * buffer, const size_t size); // 发送二进制   
        
        //接收服务端的报文，成功返回true，失败返回false。
        //buffer-存放接收到的报文的内容，maxlen-本次接收报文的最大长度。
        bool recv(string &buffer,const size_t maxlen);
        
        bool close();


        // 发送  文件到服务端
        bool sendfile(const string &filename, const size_t filesize);
        
        ~ctcpclient(){
            close();
        };                                                  // 析构函数
};

int client_main(int argc,char *argv[]);

#endif



