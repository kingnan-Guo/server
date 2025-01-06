#include "Buffer.h"



Buffer::Buffer(uint16_t sep): sep_(sep)
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
void Buffer::appendWithSep(const char* data, size_t size){

    // 无分隔符
    if (sep_==0){
        buffer_.append(data,size);                    // 处理报文内容。
    }
    // 四字节的报头
    else if (sep_ == 1) {
        buffer_.append((char*)&size, 4);   // 处理报文长度（头部）。
        // buffer_.append((char*)"HEAD", 4);  
        buffer_.append(data, size);         // 处理报文内容。
    }
    // http 协议 的 \r\n\r\n"分隔符
    else if (sep_ == 2){
        buffer_.append(data, size);         // 处理报文内容。
        buffer_.append("\r\n\r\n", 4);         // 处理报文内容。
    }

    // // buffer_.append((char*)&size, 4);   // 处理报文长度（头部）。
    // buffer_.append((char*)"HEAD", 4);   // 处理报文长度（头部）。
    // buffer_.append(data, size);         // 处理报文内容。
}



bool Buffer::pickMessage(std::string &data){
    if(buffer_.size() == 0){
        return false;
    }
    // 无分隔符
    if(sep_ == 0){
        data = buffer_;
        buffer_.clear();
    }
    // 定制的 报文头是 4 个字节
    else if(sep_ == 1){
        //// 这段是 定制的 报文头是 4 个字节 ///////////////// 之后要改成 Buffer类
        // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
        int len;
        memcpy(&len, buffer_.data(), 4);     // 从buffer_中获取报文头部。
        // 如果buffer_中的数据量小于报文头部，说明buffer_中的报文内容不完整。
        if (buffer_.size() < len + 4){
            return false;
        };
        data = buffer_.substr(4,len);   

        buffer_.erase(0, len + 4);                                 // 从buffer_中删除刚才已获取的报文。
        ////////////////////////////////////////////////////////////
    } 
    // http 协议 的 \r\n\r\n"分隔符
    else if(sep_ == 2){
        
    }

    return true;
}






// 测试数据
// int BUFFER_MAIN()
// {
//     std::string s1="aaaaaaaaaaaaab";
//     Buffer buf(1);
//     buf.appendWithSep(s1.data(),s1.size());
//     std::string s2;
//     buf.pickMessage(s2);
//     printf("s2=%s\n",s2.c_str());
// }
// g++ -g -o test Buffer.cpp