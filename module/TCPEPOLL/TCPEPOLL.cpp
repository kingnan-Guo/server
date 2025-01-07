#include "TCPEPOLL.h"


  //  1、首先设置 2 和 15 的信号
  //  2、在信号处理函数中 停止主从事件 循环 和工作线程
  //  3、服务程序 主动退出

EchoServer* echoserver;


void stop(int sig){
  printf("sig %d\n", sig);



  //  调用 EchoServer 类中的 Stop 函数
  echoserver->Stop();
  printf("echoserver stop \n");
  delete echoserver;
  printf("delete echoserver \n");
  exit(0);

}


int  TCPEPOLL(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }







    //  2 和 15 的信号
    signal(SIGTERM, stop);  // 信号 15  SIGTERM ， 系统 kill 或 killall 命令发送的信号
    signal(SIGINT, stop);  // 信号 2 SIGINT ， ctrl + c 发送的信号







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
    echoserver = new EchoServer(argv[1],atoi(argv[2]), 3, 3);
    // EchoServer echoserver(argv[1],atoi(argv[2]), 3, 0);

    echoserver->Start();      

  return 0;
}



// ps -ef | grep Reactor

// killall -9 Reactor
// 
// ps -T -p 3931


 
