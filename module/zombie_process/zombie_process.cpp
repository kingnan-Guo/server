#include <iostream>
#include <unistd.h> // 包含 fork() wait()  waitpid()
#include <sys/types.h>// 
#include <sys/wait.h> // 包含 wait() waitpid()
#include "zombie_process.h"
using namespace std;



// 僵尸进程是 一个已经终止的进程，但是它的进程描述符仍然保留在系统中，直到父进程通过调用 wait() 或 waitpid() 来获取它的终止状态。
// 僵尸进程的存在可能会导致系统资源泄漏，因此需要及时处理。


// 如果一个进程的父进程   先于 子进程 停止, 那么子进程 会 托管到  1 号进程
// 1号进程 会调用 wait() 或 waitpid() 来获取它的终止状态，从而避免僵尸进程的产生。

ZOMBIE_PROCESS::ZOMBIE_PROCESS_Class::ZOMBIE_PROCESS_Class() {
    // 可以在这里进行初始化操作
    std::cout << "ZOMBIE_PROCESSger initialized." << std::endl;
}

// ZOMBIE_PROCESS 方法的实现
void ZOMBIE_PROCESS::ZOMBIE_PROCESS_Class::ZOMBIE_PROCESS_info() {
    std::cout << "ZOMBIE_PROCESS_info: "<< std::endl;
}


void ZOMBIE_PROCESS_init(){
    std::cout << "ZOMBIE_PROCESS initialized." << std::endl;
}



// 父进程 先于 子进程结束
// 子进程  ps -ef | grep ./sylar :  root     14482     1  0 16:41 pts/3    00:00:00 ./sylar
int test1(){
    if(fork() == 0){
        // 子进程
        std::cout << "Child process." << std::endl;

        for (size_t i = 0; i < 100; i++)
        {
            std::cout << "Child process ing " << i << std::endl;
            sleep(1);
        }
        return 0;
        
    }else{
        // 父进程
        std::cout << "Parent process." << std::endl;

        sleep(20); // 等待子进程结束
        std::cout << "Parent process end" << std::endl;

        return 0;

    }
}



int test2(){
    if(fork() == 0){
        // 子进程
        std::cout << "Child process." << std::endl;

        sleep(5);
        // int * p = 0 ; *p = 1; //可以产生异常退出; 信号为：11  ,是内存泄漏
        exit(0);
        
        return 0;
        
    }else{
        // 父进程
        std::cout << "Parent process." << std::endl;
        int sts;

        pid_t pid = wait(&sts); // 等待子进程结束
        std::cout << "已经 终止 的子进程编号是 == "<< pid << std::endl;

        if(WIFEXITED(sts)){
            std::cout << "子进程正常退出，退出码为：" << WEXITSTATUS(sts) << std::endl;
        } else if(WIFSIGNALED(sts)){
            std::cout << "子进程异常退出，信号为：" << WTERMSIG(sts) << std::endl;
        }
        
        std::cout << "Parent process end" << std::endl;
        return 0;

    }
}



void test3_func(int sig){

    std::cout << "信号  "<< sig << std::endl;
    int sts;

    pid_t pid = wait(&sts); // 等待子进程结束
    std::cout << "捕获到 已经 终止 的子进程编号是 == "<< pid << std::endl;

    if(WIFEXITED(sts)){
        std::cout << "捕获到 子进程正常退出，退出码为：" << WEXITSTATUS(sts) << std::endl;
    } else if(WIFSIGNALED(sts)){
        std::cout << "捕获到 子进程异常退出，信号为：" << WTERMSIG(sts) << std::endl;
    }
}



int test3(){

    signal(SIGCHLD, test3_func); // 捕获子进程退出的信号

    if(fork() == 0){
        // 子进程
        std::cout << "Child process." << std::endl;

        sleep(5);
        // int * p = 0 ; *p = 1; //可以产生异常退出; 信号为：11  ,是内存泄漏
        exit(1);
        return 0;
        
    }else{
        // 父进程
        

        while (1)
        {
            std::cout << "Parent process ing" << std::endl;
            sleep(1);
        }
        

        
        std::cout << "Parent process end" << std::endl;
        return 0;

    }
}




int ZOMBIE_PROCESS_MAIN(){
    // test1();
    // test2();
    test3();
}
