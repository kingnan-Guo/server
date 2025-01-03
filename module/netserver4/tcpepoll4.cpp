#include "EchoServer.h"

int tcpepoll4(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    EchoServer echoserver(argv[1],atoi(argv[2]));

    echoserver.Start();      

  return 0;
}


