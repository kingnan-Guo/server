#include <iostream>
#include <stdio.h>
#include "muliti_process.h"
#include <unistd.h> // 包含 fork() wait()  waitpid()
#include <sys/types.h>// 
#include <sys/wait.h> // 包含 wait() waitpid()
using namespace std;

MULITI_PROCESS::MULITI_PROCESS_Class::MULITI_PROCESS_Class() {
    // 可以在这里进行初始化操作
    std::cout << "MULITI_PROCESSger initialized." << std::endl;
}

// MULITI_PROCESS 方法的实现
void MULITI_PROCESS::MULITI_PROCESS_Class::MULITI_PROCESS_info() {
    std::cout << "MULITI_PROCESS_info: "<< std::endl;
}


void MULITI_PROCESS_init(){
    std::cout << "MULITI_PROCESS initialized." << std::endl;
}



void ParentEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN); 


    std::cout << "ParentEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;
    kill(0, SIGTERM); // 向所有子进程发送 SIGTERM 15 信号,通知全部的子进程 退出; kill 0  表示向所有子进程发送信号 也会发给自己

    // 释放资源


    exit(0);
};
void ChildEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN);



    std::cout << "ChildEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;

    // 释放资源 只释放子进程

    exit(0);
};

void  MULITI_PROCESS_test1(){
    // 忽略全部 64 个信号
    for (size_t i = 1; i < 64; i++)
    {
        signal(i, SIG_IGN); // 忽略信号
    }

    signal (SIGTERM, ParentEXIT); // 15  终止进程
    signal (SIGINT, ParentEXIT); //  2  中断进程



    // while (1)
    // {
        // 父进程  每过 5s 创建 一个子 进程
        if(fork() > 0){
            // 父进程
            std::cout << "Parent process ing  pid = " << getpid() << std::endl;
            sleep(100);
            //continue;
        }else{
            // 子进程
            std::cout << "Child process" << std::endl;
            
            signal (SIGTERM, ChildEXIT); // 子进程的 退出函数 与 父进程 不一样
            signal (SIGINT, SIG_IGN);// 子进程 不需要 捕获 SIGINT 信号, 忽略


            while (1)
            {
                std::cout << "Child process ing  pid = " << getpid() << std::endl;
                sleep(1);
                continue;
            }
            

        }

    // }



}


// 清除 主进程  kill 123  ,子进程 也会 去清除
int MULITI_PROCESS_MAIN(){
    MULITI_PROCESS_test1();
}
