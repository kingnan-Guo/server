#ifndef __SYLAR_SHARED_MEMORY_H
#define __SYLAR_SHARED_MEMORY_H

// 定义命名空间
namespace SHARED_MEMORY {
    class SHARED_MEMORY_Class {
    public:
        SHARED_MEMORY_Class();
        void SHARED_MEMORY_info();
    };
}

void SHARED_MEMORY_init();
int SHARED_MEMORY_MAIN();

#endif