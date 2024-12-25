#ifndef __SYLAR_NETWORK_H
#define __SYLAR_NETWORK_H
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
using namespace std;


class ctcpserver//TCP通讯的服务端类。
{
    private:
        int m_listenfd;//监听的socket，-1表示未初始化。
        int m_clientfd;//客户端连上来的socket，-1表示客户端未连接。
        string m_clientip;//客户端字符串格式的IP。
        unsigned short m_port;//服务端用于通讯的端口。
    public:
        ctcpserver():m_listenfd(-1), m_clientfd(-1),m_clientip(""), m_port(0){};

        bool initserver(const unsigned short in_port); //初始化服务端用于监听的socket。

        bool accept();
        bool send(const string &buffer);
        bool recv(string &buffer,const size_t maxlen);



        //获取客户端的IP(字符串格式)。
        // const string &clientip() const
        // {
        //     return m_clientip;
        // }




        // 关闭监听的socket。
        bool closelisten()
        {
            if (m_listenfd == -1){
                return false;
            }; 
            ::close(m_listenfd);
            m_listenfd = -1;
            return true;
        }

        //关闭客户端连上来的socket。
        bool closeclient()
        {
            if(m_clientfd == -1){
                return false;
            };
            ::close(m_clientfd);
            m_clientfd = -1;
            return true;
        }

        ~ctcpserver(){
            closelisten(); 
            closeclient(); 
        };


};

int network_main(int argc,char *argv[]);

#endif