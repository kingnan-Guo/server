#include <iostream>
#include <stdio.h>
// #include "tcpepoll3.h"
#include "tcpepoll4.h"
#include "ThreadPool.h"
// #include "EVENTFD.h"
// #include "PIPE.h"
#include "timerfdAndSigefdDemo.h"
#include "TimeStamp.h"


//  ./Reactor 172.27.124.240  8080
int main(int arch, char* argv[])
{
    printf("Reactor star \r\n");

    // tcpepoll3(arch, argv); // TcpServer DEMO
    tcpepoll4(arch, argv);  // EchoServer DEMO

    // ThreadPool_MAIN();   // 线程池

    // EVENTFD_MAIN(arch, argv); // 事件fd
    // PIPE_MAIN(arch, argv);   // 管道

    // 定时器 fd 和 信号 fd 的 DEMO
    // timerfdAndSigefdDemo(arch, argv);

    // 定时器
    // TIMESTAMP_MAIN();
    return 0;
}



