#include <iostream>
#include "exit_t.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * retrun 表示函数返回，会调用局部对象的析构函数， main 中的 return 会调用全局 对象的 析构函数
 * exit() 表示中止进程， 不会调用局部对象的析构函数， 只调用全局对象的析构函数
 * exit() 和 return 的区别在于， exit() 会调用全局对象的析构函数， 而 return 不会调用全局对象的析构函数
 * exit() 会执行清理工作，然后退出， _exit() he  _Exit() 直接退出 ， 不会执行清理工作
 * 
 * 可以用  atexit() 函数登记种植函数 最多 32，这些 函数 有  exit(0) 自动调用
 * int atexit(void (*function)(void));
 * 
*/

EXIT_T::EXIT_T_Class::EXIT_T_Class() {
    // 可以在这里进行初始化操作
    std::cout << "EXIT_Tger initialized." << std::endl;
}

// EXIT_T 方法的实现
void EXIT_T::EXIT_T_Class::EXIT_T_info() {
    std::cout << "EXIT_T_info: "<< std::endl;
}

void EXIT_T_init(){
    std::cout << "EXIT_T initialized." << std::endl;
}



struct test
{
    char name;
    test(char data):name(data){
        std::cout << "调用 构造函数 " << name <<  std::endl;
    };
    // 析构函数
    ~test(){
        std::cout << "调用 析构函数 "<< name << std::endl;
    };
};


// test aa('a');

int EXIT_T_MAIN(){




    test aa('b');
    
    // exit(0);
    std::cout << "EXIT_T_MAIN:  \r\n" << std::endl;
    

    return 0;
}
