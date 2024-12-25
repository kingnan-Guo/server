#include <iostream>
#include <stdio.h>
#include "sharedMemory.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

// 在多线程共享进程的地址空间,如果 多个线程需要i访问同一块内存,需要加锁,否则会出现数据错乱, 用全局变量就可以

// 多几进程中 ，每个进程空间盾立不共享，如果多个进程需要访问同一块内存，不能用全局变量 只能用  共享内存

// 共享内存  允许多个进程 ， 访问同一块内存空间， 是多个进程之间和传递数据最高效的方式。进程可以将共享内存 连接到他们自己的地址空间中  ，修改了共享内存中的数据，其他的 进程读到的数据也会改变
// 贡献内存没有提供 锁机制，也就是说 ，在阴谋一个进程对共享内存进项读写的时候，不会阻止其他进程 对它的 读写， 如果对共享内存读写 加锁，可以使用信号量






SHARED_MEMORY::SHARED_MEMORY_Class::SHARED_MEMORY_Class() {
    // 可以在这里进行初始化操作
    std::cout << "SHARED_MEMORYger initialized." << std::endl;
}

// SHARED_MEMORY 方法的实现
void SHARED_MEMORY::SHARED_MEMORY_Class::SHARED_MEMORY_info() {
    std::cout << "SHARED_MEMORY_info: "<< std::endl;
}


void SHARED_MEMORY_init(){
    std::cout << "SHARED_MEMORY initialized." << std::endl;
}



// 只能使用 c++ 内置的数据类型，不能使用自定义的数据类型， 不能采用 STL 容器 ，也不能使用移动语义，不能使用智能指针，
//  如果 要实现多进程的 生产 消费者 ，只能采用 循环队列
struct sharedMemoryData
{
    int num;
    char name[20];
};




//shmget 函数， 创建/ 获取 共享内存
// 查看共享内存命令 ipcs -m
//  查看共享内存命令 ipcrm -m
int SHARED_MEMORY_test1(){

    //1 shmget 函数， 创建/ 获取 共享内存
    // 键值 key 为 0x5005， 也可用其他值，但需要保证唯一性, 16进制
    int shmid = shmget(0x5005, sizeof(sharedMemoryData), 0640 | IPC_CREAT);


    if(shmid < 0){
        perror("shmget error");
        return -1;
    }

    
    std::cout << "SHARED_MEMORY shmid = " << shmid<< std::endl;

    // 2、shmat 函数，将共享内存连接到进程的地址空间
    sharedMemoryData *sharedMemory = (sharedMemoryData *)shmat(shmid, NULL, 0);
    if(sharedMemory == (void *)-1){
        perror("shmat error");
        return -1;
    }

    // 3、
    std::cout << "原来的值  num  = " << sharedMemory->num   << " name = " << sharedMemory->name << std::endl;

    // sharedMemory->num = 100;
    // strcpy(sharedMemory->name, "hello world");

    sharedMemory->num = atoi("100");
    strcpy(sharedMemory->name, "hello world" + atoi("10") );

    std::cout << "修改后的值  num  = " << sharedMemory->num   << " name = " << sharedMemory->name << std::endl;


    // 4、shmdt 函数，将共享内存从进程的地址空间分离
    if(shmdt(sharedMemory) < 0){
        perror("shmdt error");
        return -1;
    }


    // 5、shmctl 函数，删除共享内存
    if(shmctl(shmid, IPC_RMID, NULL) < 0){
        perror("shmctl error");
        return -1;
    }
}



// shmat 函数，将共享内存连接到进程的地址空间
void SHARED_MEMORY_test2(){

    
}


// shmdt 函数，将共享内存从进程的地址空间分离


int SHARED_MEMORY_MAIN(){
    SHARED_MEMORY_test1();
}
