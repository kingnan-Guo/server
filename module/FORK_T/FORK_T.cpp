#include <iostream>
#include "fork_t.h"
#include <unistd.h>
#include <string>
#include <fstream>
using namespace std;



//  0号进程  系统进程是所有进程 的祖先吗,它创建了 1 号 和 2 号
// 1 号进程 systemd 负责执行内核的初始化工作 和 进行系统配置， 然后启动其他服务
// 2 号进程 kthreadd 负责所有内核线程的调度和管理
// 使用 pstree 命令可以查看进程树  yum -y install psmisc

// 调用fork  可以创建一个新进程

// shell 是 命令解释 器， 输入一个命令， shell 就会 fork 一个子进程， 然后执行命令
// 例如  shell 输入 ls 命令， shell 就会 fork 一个子进程， 然后执行 ls 命令

FORK_T::FORK_T_Class::FORK_T_Class() {
    // 可以在这里进行初始化操作
    std::cout << "FORK_Tger initialized." << std::endl;
}

// FORK_T 方法的实现
void FORK_T::FORK_T_Class::FORK_T_info() {
    std::cout << "FORK_T_info: "<< std::endl;
}


void FORK_T_init(){
    std::cout << "FORK_T initialized." << std::endl;
}



void FORK_T_test1(){
    int b = 7;

    // const char *a = "hello world";
    std::string str = "Hello, World!";

    // fork() 创建一个子进程， 调用一次 但是返回两次， 两次返回值的区别是紫禁城的返回值 是 0 ， 父进程的返回值是子进程的 id
    // 子进程 和 父进程继续执行 fork() 后的代码, 子进程是父进程的 副本
    //子进程获得了 父进程数据空间、堆和栈的副本（注意： 子进程拥有的是副本，不是共享，但是地址 是一样的，因为地址都是 虚拟地址 ，故意改成一样，不是 寄存器的地址 ）
    // fork 之后 子父进程 的执行顺序是不确定的
    pid_t pid = fork();

    // 父进程 pid 是  子进程 的 od
    if(pid > 0){
        sleep(1);
        std::cout << "父进程 pid == " << pid << std::endl;
        std::cout << "父进程 b == " << b << " &b "<< &b  << "  str == " << str   << " &str " << &str << std::endl;
    } else {
        str = "Hello, World! from child process"; // 修改子进程的值
        b = 8;
        std::cout << "子进程 pid == " << pid << std::endl;
        std::cout << "子进程 b == " << b << " &b "<< &b  << "  str == " << str   <<  " &str " << &str << std::endl;
    }
}


// 进程要执行里一个程序，这种方法再 shell 常见， 子进程从 fork() 返回后，调用 exec 函数，exec 函数的作用是 加载新的程序，替换当前进程的正文段、数据段、堆和栈
void FORK_T_test2(){
    int b = 7;

    // const char *a = "hello world";
    std::string str = "Hello, World!";

    // fork() 创建一个子进程， 调用一次 但是返回两次， 两次返回值的区别是紫禁城的返回值 是 0 ， 父进程的返回值是子进程的 id
    // 子进程 和 父进程继续执行 fork() 后的代码, 子进程是父进程的 副本
    //子进程获得了 父进程数据空间、堆和栈的副本（注意： 子进程拥有的是副本，不是共享，但是地址 是一样的，因为地址都是 虚拟地址 ，故意改成一样，不是 寄存器的地址 ）
    // fork 之后 子父进程 的执行顺序是不确定的
    pid_t pid = fork();

    // 父进程 pid 是  子进程 的 od
    if(pid > 0){
        while (1)
        {
            sleep(1);
            std::cout << "父进程 pid == " << pid << std::endl;
        }
        

    } else {
  
        sleep(10);    
        std::cout << "子进程 调用 execl  ;  pid == " << pid << std::endl;
        execl("/bin/ls", "/bin/ls", "-lt", "/tmp", 0);// 替换当前 进程   ，对 父进程 没有影响

        std::cout << "子进程 结束  退出  pid == " << pid << std::endl;// 这段代码不会执行的原因 是因为上面的 execl 执行 后 会替换现在的进程
    }
}





// fork 共享文件 ==================
// fork 的一个特性是 在父进程中打开的文件描述符 都会被复制到子进程中 ,父进程 和子进程 共享同一个 文件偏移量
//  如果 父进程 和子进程写统一描述符指向的文件,担忧没有任何形式的同步,那么 他们的输出 可能会互相混合; 解决 这个问题的方式  进程同步
void FORK_T_test3(){
    ofstream fout;
    fout.open("/tmp/test1.txt", ios::out);
    fork();
    for (size_t i = 0; i < 100; i++)
    {
        fout << " 进程 "  << getpid() <<  " i = " << i  << endl;
    }
    fout.close();
}


//  vfork ===================
//  vfork() 和 fork() 相同的地方是 ,函数使用方法相同, 返回值相同
// vfork() 和 fork() 的区别是 vfork() 不复制父进程的 数据段、堆和栈
// vfork 是创建一个新的 进程, 二改新进程的目的是 exec 一个新程序 ,它不复制 父进程的地址 空间, 因为 子进程会立即调用 exec, 于是也不会使用 父进程的地址空间.,如果子进程使用了 父进程的 地址空间,可能会带来未知的后果
// vfork() 的一个特性是 子进程 必须调用 exec 或 exit, 否则 子进程会一直阻塞, 父进程会一直等待子进程调用 exec 或 exit
// vfork() 的一个特性是 vfork() 保证子进程执行, 在子进程 调用 exec 或 exit 之后 ,父进程 才回复


int FORK_T_MAIN(){

    // FORK_T_test2();
    FORK_T_test3();



    
}
