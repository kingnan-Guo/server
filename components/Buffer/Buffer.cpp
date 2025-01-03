#include "Buffer.h"



Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}


// 把数据追加到 buffer_ 中
void Buffer::append(const char* data, int size){
    buffer_.append(data, size);
};

// erase 清除指定位置指定长度
void Buffer::erase(size_t start, size_t size){
    buffer_.erase(start, size);
};


// 返回 buffer_ 的大小
size_t Buffer::size(){
    return buffer_.size();
};

// 返回 buffer_ 的指针， 首地址           
const char * Buffer::data(){
    return buffer_.data();
};

void Buffer::clear(){
    buffer_.clear();
};


// 把数据追加到buf_中，附加报文头部。
void Buffer::appendWithHearder(const char* data, size_t size){
    buffer_.append((char*)&size, 4);   // 处理报文长度（头部）。
    // buffer_.append((char*)"HEAD", 4);   // 处理报文长度（头部）。
    buffer_.append(data, size);         // 处理报文内容。
}


