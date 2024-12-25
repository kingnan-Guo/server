#ifndef __SYLAR_PUBLIC_H
#define __SYLAR_PUBLIC_H

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>


using namespace std;

// 循环队列 ; 模板创建一个循环队列可以让队列支持不同的数据类型。 
template <class TT, int MaxLength>


class squeue{
    private:
        bool m_inited; // 队列被出示化编制  ， true 表示被初始化，false 表示未初始化
        TT *m_data[MaxLength]; // 用数组 存储 循环队列 中的 元素
        int m_head; // 队头
        int m_tail; // 队尾
        int m_length; // 队列中元素的个数
        squeue(const squeue<TT, MaxLength> &); // 禁止拷贝构造
        squeue<TT, MaxLength> &operator=(const squeue<TT, MaxLength> &) = delete; // 禁止赋值操作
    public:
        // 构造函数
        squeue(){
            init();
        };
        ~squeue();
        void init(){
            if(m_inited != true){
                m_head = 0;
                m_tail = ;
                m_length = 0;

                memset(m_data, 0, sizeof(m_data));
                m_inited = true;
            }
        };

}


#endif