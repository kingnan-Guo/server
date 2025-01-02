#include "Buffer.h"



Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}


// 把数据追加到 buffer_ 中
void Buffer::append(char* data, int size){
    buffer_.append(data, size);
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


