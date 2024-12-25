#include <iostream>
#include <stdio.h>
#include "Semaphore.h"

// 信号量的本质是 一个非负数 >=0 的计数器
// /用于给 贡献资源建立一个 标志， 表示该贡献资源被占用 情况

// 信号量 机制 是一种 基于PV操作的 同步互斥 机制
// P 操作： （wait）将信号量的值减 1，如果信号量的值为 0，将阻塞等待直到信号量大于 0         申请资源， 如果资源数大于0， 则申请成功， 资源数减1； 如果资源数等于0， 则申请失败， 等待资源
// V 操作： （signal）将信号量的值加 1，任何时候都不会阻塞；    如果此时有进程在等待该信号量，则唤醒其中一个进程      释放资源， 将资源数加1

// 如果 约定信号量的取值只是0 和 1（0 资源不可用 1 资源可用），可以实现互斥锁
// 如果约定信号量的 取值 表示 可用资源的数量，可以实现 产生者 / 消费者 模型


SEMAPHORE::SEMAPHORE_Class::SEMAPHORE_Class() {
    // 可以在这里进行初始化操作
    std::cout << "SEMAPHOREger initialized." << std::endl;
}

// SEMAPHORE 方法的实现
void SEMAPHORE::SEMAPHORE_Class::SEMAPHORE_info() {
    std::cout << "SEMAPHORE_info: "<< std::endl;
}


void SEMAPHORE_init(){
    std::cout << "SEMAPHORE initialized." << std::endl;
}


int SEMAPHORE_MAIN(){
    
}
