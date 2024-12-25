#include <iostream>
#include <stdio.h>
#include <stdlib.h>
// #include "log.h"
#include "testVector.h"
#include "getcwd_t.h"
// #include "print_t.h"

// #include "exit_t.h"
#include "signal_t.h" // 信号
#include "system_exec.h"

#include "fork_t.h"

#include "zombie_process.h"

#include "muliti_process.h"


#include "sharedMemory.h"

#include "CircularQueue.h"


#include "network.h"

#include "client.h"


#include "networkMultiProcess.h"

// 发送文件
#include "clientFile.h"
#include "serverFile.h"

#include "clientselect.h"
#include "tcpselect.h"

// epoll 模型 
#include "tcpepoll_horizontal_edge_trigger.h" // 边沿触发 水平触发
#include "epoll_client.h" // epoll 模型 客户端



int main(int arch, char* argv[])
{
    printf(" star sylar \r\n");
    // LOGGER_MAIN(); // 日志

    // testVector();

    // getcwd_main(arch, argv);

    // signal_t_MAIN();

    // EXIT_T_MAIN();

    // SYSTEM_EXEC_MAIN();

    // FORK_T_MAIN();


    // ZOMBIE_PROCESS_MAIN();

    // MULITI_PROCESS_MAIN();


    // SHARED_MEMORY_MAIN();


    // network_main(arch, argv);
    // client_main(arch, argv);



    // NETWORK_MULTI_PROCESS_MAIN(arch, argv);


    // 发送文件相关
    //CLIENT_FILE_MAIN(arch, argv);
    // SERVER_FILE_MAIN(arch, argv);


    // 连接
    // TCP_SELECT_MAIN(arch, argv);
    // CLIENT_SELECT_MAIN(arch, argv);


    // epoll
    // TCP_EPOLL_MAIN(arch, argv); // epoll 模型
    TCP_EPOLL_HET_MAIN(arch, argv); //  边沿触发 水平触发 
    // TCP_EPOLL_CLIENT_MAIN(arch, argv);

    return 0;
}

