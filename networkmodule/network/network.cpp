/*
 * 程序名：demo2.cpp，此程序用于演示socket通信的服务端
*/
#include "network.h"

// #include <iostream>
// #include <cstdio>
// #include <cstring>
// #include <cstdlib>
// #include <unistd.h>
// #include <netdb.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
using namespace std;
 

//  //./sylar_server  8080
// int network_main(int argc,char *argv[])
// {
//   if (argc!=2)
//   {
//     cout << "Using:./demo2 通讯端口\nExample:./demo2 5005\n\n";   // 端口大于1024，不与其它的重复。
//     cout << "注意：运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
//     cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
//     return -1;
//   }

//   // 第1步：创建服务端的socket。 
//   // AF_INET表示使用IPv4协议，
//   // SOCK_STREAM 表示使用面向连接的TCP协议。 面向连接 socket 1、 数据不会调式 2 数据不会错乱， 双向通道 ； SOCK_DGRAM表示使用无连接的UDP协议。
//   // 0表示使用默认协议。最终使用的协议是TCP协议。  SOCK_STREAM ： IPPROTO_TCP  ， SOCK_DGRAM ： IPPROTO_UDP
//   int listenfd = socket(AF_INET,SOCK_STREAM,0);
//   if (listenfd==-1) 
//   { 
//     perror("socket"); return -1; 
//   }
  
//   // 第2步：把服务端用于通信的IP和端口绑定到socket上。 
//   struct sockaddr_in servaddr;          // 用于存放服务端IP和端口的数据结构。
//   memset(&servaddr,0,sizeof(servaddr));
//   servaddr.sin_family = AF_INET;        // 指定协议。
//   servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 服务端任意网卡的IP都可以用于通讯。
//   servaddr.sin_port = htons(atoi(argv[1]));     // 指定通信端口，普通用户只能用1024以上的端口。
//   // 绑定服务端的IP和端口。
//   if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0 )
//   { 
//     perror("bind"); close(listenfd); return -1; 
//   }
 
//   // 第3步：把socket设置为可连接（监听）的状态。
//   if (listen(listenfd,5) != 0 ) 
//   { 
//     perror("listen"); close(listenfd); return -1; 
//   }
 
//   // 第4步：受理客户端的连接请求，如果没有客户端连上来，accept()函数将阻塞等待。
//   int clientfd=accept(listenfd,0,0);
//   if (clientfd==-1)
//   {
//     perror("accept"); close(listenfd); return -1; 
//   }

//   cout << "客户端已连接。\n";
 
//   // 第5步：与客户端通信，接收客户端发过来的报文后，回复ok。
//   char buffer[1024];
//   while (true)
//   {
//     int iret;
//     memset(buffer,0,sizeof(buffer));
//     // 接收客户端的请求报文，如果客户端没有发送请求报文，recv()函数将阻塞等待。
//     // 如果客户端已断开连接，recv()函数将返回0。
//     if ( (iret=recv(clientfd,buffer,sizeof(buffer),0))<=0) 
//     {
//        cout << "iret=" << iret << endl;  break;   
//     }
//     cout << "接收：" << buffer << endl;
 
//     strcpy(buffer,"ok");  // 生成回应报文内容。
//     // 向客户端发送回应报文。
//     if ( (iret=send(clientfd,buffer,strlen(buffer),0))<=0) 
//     { 
//       perror("send"); break; 
//     }
//     cout << "发送：" << buffer << endl;
//   }
 
//   // 第6步：关闭socket，释放资源。
//   close(listenfd);   // 关闭服务端用于监听的socket。
//   close(clientfd);   // 关闭客户端连上来的socket。
// }


// limit -a 查看 信息

// 单个线程中打开的 socker 数量不能超过 1024， 超过 会报错

// socker 的创建 读取文件是同类 ， 可以使用  read wrte 替换 recv send 



bool ctcpserver::initserver(const unsigned short in_port){


    // 第1步：创建服务端的socket。
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd == -1) 
    { 
      perror("socket"); 
      return false; 
    }

    m_port = in_port;


    //第2步：把服务端用于通信的IP和端口绑定到socket上。
    struct sockaddr_in servaddr;//用于存放协议、端口和IP地址的结构体。
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;//协议族，固定填AF_INET。
    servaddr.sin_port = htons(m_port);//指定服务端的通信端口。
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//如果操作系统有多个IP，全部的IP都 可以用于通讯。
    //绑定服务端的IP和端口（为socket分配IP和端口）。
    if(bind(m_listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {

        perror("bind");
        close(m_listenfd);
        m_listenfd = -1;
        return false;
    }


    //第3步：把socket设置为可连接（监听）的状态。
    if(listen(m_listenfd, 5) == -1)
    {
        perror("listen");
        close(m_listenfd);
        m_listenfd = -1;
        return false;
    }

    return true;

}


// bool ctcpserver::accept()
// {
//     struct sockaddr_in caddr;//客户端的地址信息。
//     socklen_t addrlen = sizeof(caddr);//structsockaddr_in的大小。
//     if( (m_clientfd = ::accept(m_listenfd, (struct sockaddr*) &caddr, &addrlen)) == -1){
//       return false;
//     }
    
//     m_clientip = inet_ntoa(caddr.sin_addr);//把客户端的地址从大端序转换成字符串。
//     return true;
// }



  // 受理客户端的连接（从已连接的客户端中取出一个客户端），
  // 如果没有已连接的客户端，accept()函数将阻塞等待。
bool ctcpserver::accept(){
    struct sockaddr_in caddr;        // 客户端的地址信息。  
    socklen_t addrlen = sizeof(caddr); // struct sockaddr_in的大小。
    m_clientfd = ::accept(m_listenfd, (struct sockaddr *) &caddr, &addrlen);

    cout << "accept  m_clientfd " << m_clientfd << endl;
    
    if (m_clientfd == -1){
      perror("accept()");
      close(m_clientfd);
      return false;
    };
    m_clientip = inet_ntoa(caddr.sin_addr);  // 把客户端的地址从大端序转换成字符串。
    return true;
}




bool ctcpserver::send(const string &buffer){
    if (m_clientfd==-1){
        return false;
    };

    if ( (::send(m_clientfd, buffer.data(), buffer.size(), 0)) <= 0 ){
        return false;
    };

    if ( (::send(m_clientfd,buffer.data(),buffer.size(),0))<=0){
        return false;
    };

    return true;
}



  // 接收对端的报文，成功返回true，失败返回false。
  // buffer-存放接收到的报文的内容，maxlen-本次接收报文的最大长度。
bool ctcpserver::recv(string &buffer,const size_t maxlen)
{ 
    buffer.clear();         // 清空容器。
    buffer.resize(maxlen);  // 设置容器的大小为maxlen。
    int readn = ::recv(m_clientfd, &buffer[0], buffer.size(), 0);  // 直接操作buffer的内存。
    if (readn <= 0) { 
      buffer.clear(); 
      return false; 
    }
    buffer.resize(readn);   // 重置buffer的实际大小。
    return true;
}









int network_main(int argc,char *argv[]){

    if (argc!=2)
    {
        cout << "Using:./demo2 通讯端口\n Example:./demo2 5005 \n\n";   // 端口大于1024，不与其它的重复。
        cout << "注意: 运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
        cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
        return -1;
    }

    ctcpserver tcpserver;
    if (tcpserver.initserver(atoi(argv[1])) == false) // 初始化服务端用于监听的socket。
    {
      perror("initserver()"); 
      return -1;
    }

    cout << "initserver ok\n" << endl;

    // 受理客户端的连接（从已连接的客户端中取出一个客户端），  
    // 如果没有已连接的客户端，accept()函数将阻塞等待。
    if (tcpserver.accept() == false)
    {
      perror("accept()"); 
      return -1;
    }
    // cout << "客户端已连接(" << tcpserver.clientip() << ")。\n";

    string buffer;
    while (true)
    {
      // 接收对端的报文，如果对端没有发送报文，recv()函数将阻塞等待。
      if (tcpserver.recv(buffer, 1024)==false)
      {
        perror("recv()"); break;
      }
      cout << "接收：" << buffer << endl;
      buffer = "ok";  

      if (tcpserver.send(buffer) == false)  // 向对端发送报文。
      {
        perror("send"); 
        break;
      }
      cout << "发送：" << buffer << endl;
    }

}