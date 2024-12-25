#include <iostream>
#include <stdio.h>
#include "signal_t.h"
#include "signal.h"

#include <unistd.h>


signal_t::signal_t_Class::signal_t_Class() {
    // 可以在这里进行初始化操作
    std::cout << "signal_tger initialized." << std::endl;
}

// signal_t 方法的实现
void signal_t::signal_t_Class::signal_t_info() {
    std::cout << "signal_t_info: "<< std::endl;
}


void signal_t_init(){
    std::cout << "signal_t initialized." << std::endl;
}

void func(int sig){
    std::cout << "信号 func: "<< sig << std::endl;
    signal(sig, SIG_DFL); // 恢复默认信号处理函数
}


void func1(int sig){
    std::cout << "信号 14 alarm: "<< sig << std::endl;
    signal(sig, SIG_DFL); // 恢复默认信号处理函数
}



void signal_function(){
    signal(1, func);
    signal(15, func);
    signal(2, SIG_IGN); // 忽略信号

    signal(9, func); //  无效
    signal(9, SIG_IGN); // 忽略信号,但是  9 无法 被忽略，此代码无效

    //  #include <unistd.h>
    alarm(5); // 5 秒后发送 SIGALRM 信号； 5 秒后将向 进程发送 SIGALRM 信号， 14 的信号
    signal(14, func1); // 14 是 SIGALRM 信号, 设定 定时任务 



}


// exit 表示 

void EXIT(int sig){
    std::cout << "信号 EXIT: "<< sig << std::endl;
    exit(0);
}


void exit_function(){
    // 忽略全部 64 个信号
    for (size_t i = 1; i < 64; i++)
    {
        signal(i, SIG_IGN); // 忽略信号
    }

    signal (2, EXIT); 
    signal (2, EXIT); //  无效
}


// 使用 kilall -9 ./sylar 来终止程序
// 使用 kilall -1 ./sylar 触发 func
// 使用 kill -15 ./sylar 触发 func
// 使用 kill -2 ./sylar  忽略
void signal_t_MAIN(){

    // signal_function();
    exit_function();

    while (1)
    {
        std::cout << " 执行任务  ing "<< std::endl;
        sleep(1);
    }
}
