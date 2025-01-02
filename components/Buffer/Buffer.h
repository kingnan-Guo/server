#pragma once
#include <string>
#include <iostream>


class Buffer{
    private:
        std::string buffer_;
    public:
        Buffer();
        ~Buffer();
        void append(char* data, int size);  // 把数据追加到 buffer_ 中
        size_t size();                      // 返回 buffer_ 的大小
        const char * data();                // 返回 buffer_ 的指针， 首地址
        void clear();                    // 清空 buffer_
};
