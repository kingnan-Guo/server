#include "EchoServer.h"

int tcpepoll4(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // EchoServer echoserver(argv[1],atoi(argv[2]));
    /**
     * EchoServer 构造函数
     * params
     *  ip
     *  port
     *  IO 线程 个数
     *  work 线程个数
     * 
     */
    EchoServer echoserver(argv[1],atoi(argv[2]), 3, 2);
    // EchoServer echoserver(argv[1],atoi(argv[2]), 3, 0);

    echoserver.Start();      

  return 0;
}


