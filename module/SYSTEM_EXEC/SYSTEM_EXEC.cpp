#include <iostream>
#include <stdio.h>
#include "signal.h"
#include <unistd.h>
#include "system_exec.h"


SYSTEM_EXEC::SYSTEM_EXEC_Class::SYSTEM_EXEC_Class() {
    // 可以在这里进行初始化操作
    std::cout << "SYSTEM_EXECger initialized." << std::endl;
}

// SYSTEM_EXEC 方法的实现
void SYSTEM_EXEC::SYSTEM_EXEC_Class::SYSTEM_EXEC_info() {
    std::cout << "SYSTEM_EXEC_info: "<< std::endl;
}


void SYSTEM_EXEC_init(){
    std::cout << "SYSTEM_EXEC initialized." << std::endl;
}

void SYSTEM_TEST(){
    // system 的本质 也是  fork 一个进程出来，然后 exec 执行程序
    int ret =  system("/opt/servers/sylar/build/sylar2"); // 执行程序
    std::cout << "system() return: " << ret << std::endl;
    perror("system");
}


void EXEC_TEST(){
    int ret =  execl("./ls", "./ls", "-lt", "./tmp", 0); // 执行程序
    std::cout << "execl() return: " << ret << std::endl;
    perror("execl");
}


void EXECV_TEST(){

    const  char * args[] = {"./ls", "-lt", "/tmp", 0};
    // int ret =  execv("./ls", args); // 执行程序
    int ret = execv(args[0], (char * const *)args);
    std::cout << "execl() return: " << ret << std::endl;
    perror("execl");
}

void EXECLE_TEST_NEW(){
    //新的 进程的进程编号 与 袁锦程 相同 ，但是新进程取代了 原进程 的 代码段数据段 和 堆栈
    std::cout << "进程编号 : " << getpid() << std::endl;

    int ret = execl("./sylar2", "./sylar2", 0);
    perror("execl");
}




int SYSTEM_EXEC_MAIN(){
    // SYSTEM_TEST();
    // EXEC_TEST();
    EXECLE_TEST_NEW();
}
