#include "inetAddress.h"

InetAddress::InetAddress()
{
}

InetAddress::InetAddress(const std::string &ip, uint16_t port){
    addr_.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());      // 服务端用于监听的ip地址。
    addr_.sin_port = htons(port);               // 服务端用于监听的端口
};


// 构造函数，用于将sockaddr_in类型的地址转换为InetAddress对象, 给 addr_ 复制 addr 
InetAddress::InetAddress(const sockaddr_in addr) : addr_(addr) {

}



InetAddress::~InetAddress()
{

}


// 返回字符串 表示的地址
const char *InetAddress::ip() const{
    return inet_ntoa(addr_.sin_addr);   // 将二进制格式的 IPv4 地址（struct in_addr 类型）转换为点分十进制的字符串表示形式
}


//      返回端口号
uint16_t InetAddress::port() const{
    return ntohs(addr_.sin_port);   // 将网络字节序（大端序）转换为本地字节序（小端序）
}




// 返回addr_成员的地址，转换成了sockaddr。
const sockaddr * InetAddress::addr() const{
    return (sockaddr *) &addr_;
}



void InetAddress::setaddr (const sockaddr_in clientaddr){
    addr_ = clientaddr;
};


