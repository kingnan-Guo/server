#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int threadNum):tcpServer_(ip, port, threadNum)
{

    // 设置回调函数
    tcpServer_.setNewConnectionCallBack(
        std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1)
    );
    tcpServer_.setCloseConnectionCallBack(
        std::bind(&EchoServer::HandleClose, this, std::placeholders::_1)
    );
    tcpServer_.setErrorConnectionCallBack(
        std::bind(&EchoServer::HandleError, this, std::placeholders::_1)
    );
    tcpServer_.setOnMessageCallBack(
        std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2)
    );
    tcpServer_.setSendCompleteCallBack(
        std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1)
    );
    tcpServer_.setTimeOutCallBack(
        std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1)
    );

}

EchoServer::~EchoServer()
{
}




void EchoServer::Start(){
    tcpServer_.start();
};

 // 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(Connection* connection){
    std::cout << "New Connection Come in." << std::endl;
        std::cout << "HandleNewConnection   thread id : " << syscall(SYS_gettid) << std::endl;
};

                // 关闭客户端的连接，在TcpServer类中回调此函数。 
void EchoServer::HandleClose(Connection *connection){
    std::cout << "EchoServer conn closed." << std::endl;
};

 // 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(Connection *connection){
    std::cout << "EchoServer conn error." << std::endl;
};     

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void EchoServer::HandleMessage(Connection *connection, std::string& message){


    std::cout << "HandleMessage   thread id : " << syscall(SYS_gettid) << std::endl;
    // 在这里，将经过若干步骤的运算。
    std::cout << "HandleMessage   data: " << message << std::endl;
    // 将运算结果返回给客户端


    // / 自定义协议，报文格式为：报文长度（4字节）+报文内容。
    // int len = message.size();                   // 计算回应报文的大小。
    // std::string tmpbuf((char*)&len,4);  // 把报文头部填充到回应报文中。
    // tmpbuf.append(message);             // 把报文内容填充到回应报文中。
    // //send(connection->fd(),tmpbuf.data(),tmpbuf.size(),0);   // 把临时缓冲区中的数据直接send()出去。
    // connection->send(tmpbuf.data(),tmpbuf.size());   // 把临时缓冲区中的数据直接send()出去。

    connection->send(message.data(),message.size());
};     
// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(Connection *connection){
    std::cout << "Message send complete." << std::endl;
};     
// epoll_wait()超时，在TcpServer类中回调此函数。   
void EchoServer::HandleTimeOut(EventLoop *loop){
    std::cout << "EchoServer timeout." << std::endl;
};                    