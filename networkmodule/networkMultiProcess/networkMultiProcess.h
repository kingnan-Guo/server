#ifndef __SYLAR_NETWORK_MULTI_PROCESS_H
#define __SYLAR_NETWORK_MULTI_PROCESS_H
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h> // 包含 fork() wait()  waitpid()
#include <sys/types.h>// 
#include <sys/wait.h> // 包含 wait() waitpid()

#include <fstream>
#include <netinet/tcp.h> // 包含 TCP_NODELAY
using namespace std;


class ctcpserverMultiProcess//TCP通讯的服务端类。
{
    private:
        int m_listenfd;//监听的socket，-1表示未初始化。
        int m_clientfd;//客户端连上来的socket，-1表示客户端未连接。
        string m_clientip;//客户端字符串格式的IP。
        unsigned short m_port;//服务端用于通讯的端口。
    public:
        ctcpserverMultiProcess():m_listenfd(-1), m_clientfd(-1),m_clientip(""), m_port(0){};

        bool initserver(const unsigned short in_port); //初始化服务端用于监听的socket。

        bool accept();
        bool send(const string &buffer);
        bool recv(string &buffer,const size_t maxlen);

        bool recv(void * buffer,const size_t size);

        bool recvFile(const string &filename, const size_t filesize);


        //获取客户端的IP(字符串格式)。
        const string &clientip() const
        {
            return m_clientip;
        }




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

        ~ctcpserverMultiProcess(){
            closelisten(); 
            closeclient(); 
        };


};

int NETWORK_MULTI_PROCESS_MAIN(int argc,char *argv[]);

#endif