#pragma once
#include <string>
#include <iostream>
#include <cstring>

class Buffer{
    private:
        std::string buffer_;
        const uint16_t sep_;  // 报文的分隔符：0-无分隔符(固定长度、视频会议)；1-四字节的报头；2-"\r\n\r\n"分隔符（http协议）。
    public:
        Buffer(uint16_t sep = 2);
        ~Buffer();
        void append(const char* data, int size);  // 把数据追加到 buffer_ 中

        void appendWithSep(const char* data, size_t size);  // 把数据追加到 buffer_ 中， 并在数据前加上 header

        void erase(size_t pos, size_t size);     // 删除 buffer_ 中的数据 ; pos 表示起始位置， size 表示删除的长度
        size_t size();                      // 返回 buffer_ 的大小
        const char * data();                // 返回 buffer_ 的指针， 首地址
        void clear();                    // 清空 buffer_

        bool pickMessage(std::string &data);  // 从 buffer_ 中取出一个报文，放到 data 中， 如果 buffer_ 中没有 报文，返回false
};
