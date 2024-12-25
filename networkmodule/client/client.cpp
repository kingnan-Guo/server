/*
 * 程序名：demo1.cpp，此程序用于演示socket的客户端
*/
#include "client.h"


// #include <iostream>
// #include <cstdio>
// #include <cstring>
// #include <cstdlib>
// #include <unistd.h>
// #include <netdb.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// using namespace std;
 
// int client_main(int argc,char *argv[])
// {
//   if (argc!=3)
//   {
//     cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n"; 
//     return -1;
//   }

//   // 第1步：创建客户端的socket。  
//   int sockfd = socket(AF_INET,SOCK_STREAM,0);
//   if (sockfd==-1)
//   {
//     perror("socket"); return -1;
//   }
 
//   // 第2步：向服务器发起连接请求。 
//   struct hostent* h;    // 用于存放服务端IP的结构体。
//   if ( (h = gethostbyname(argv[1])) == 0 )  // 把字符串格式的IP转换成结构体。
//   { 
//     cout << "gethostbyname failed.\n" << endl; close(sockfd); return -1;
//   }
//   struct sockaddr_in servaddr;              // 用于存放服务端IP和端口的结构体。
//   memset(&servaddr,0,sizeof(servaddr));
//   servaddr.sin_family = AF_INET;
//   memcpy(&servaddr.sin_addr,h->h_addr,h->h_length); // 指定服务端的IP地址。
//   servaddr.sin_port = htons(atoi(argv[2]));         // 指定服务端的通信端口。
  
//   if (connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)  // 向服务端发起连接清求。
//   { 
//     perror("connect"); close(sockfd); return -1; 
//   }
  
//   // 第3步：与服务端通讯，客户发送一个请求报文后等待服务端的回复，收到回复后，再发下一个请求报文。
//   char buffer[1024];
//   for (int ii=0;ii<3;ii++)  // 循环3次，将与服务端进行三次通讯。
//   {
//     int iret;
//     memset(buffer,0,sizeof(buffer));
//     sprintf(buffer,"这是第%d个超级女生，编号%03d。",ii+1,ii+1);  // 生成请求报文内容。
//     // 向服务端发送请求报文。
//     if ( (iret=send(sockfd,buffer,strlen(buffer),0))<=0)
//     { 
//       perror("send"); break; 
//     }
//     cout << "发送：" << buffer << endl;

//     memset(buffer,0,sizeof(buffer));
//     // 接收服务端的回应报文，如果服务端没有发送回应报文，recv()函数将阻塞等待。
//     if ( (iret=recv(sockfd,buffer,sizeof(buffer),0))<=0)
//     {
//        cout << "iret=" << iret << endl; break;
//     }
//     cout << "接收：" << buffer << endl;

//     sleep(1);
//   }
 
//   // 第4步：关闭socket，释放资源。
//   close(sockfd);
// }



 //./sylar_server  8080

bool ctcpclient::connect(const string &in_ip,const unsigned short in_port){

    // 如果  socket 已经创建，则直接返回
    if(m_clientfd != -1) {
        return false;
    }
    m_ip = in_ip;
    m_port = in_port;
    //创建客户端的 socket
    if(m_clientfd = socket(AF_INET,SOCK_STREAM,0) == -1){
        return false;
    }
    m_clientfd = socket(AF_INET,SOCK_STREAM,0);
    //if (m_clientfd == -1) 
    if(m_clientfd == -1)
    {
      return false;
    }


 
    // 第2步：向服务器发起连接请求。 

    struct hostent* h;                        // 用于存放服务端IP的结构体。
    if ( (h = gethostbyname(m_ip.c_str())) == 0 )  // 把字符串格式的IP转换成结构体。
    { 
      cout << "gethostbyname failed.\n" << endl; 
      ::close(m_clientfd); 
      return -1;
    }

    struct sockaddr_in servaddr;              // 用于存放服务端IP和端口的结构体。
    memset(&servaddr, 0, sizeof(servaddr));     // 初始化结构体。
    servaddr.sin_family = AF_INET;              // 指定使用IPv4。
    memcpy(&servaddr.sin_addr, h->h_addr, h->h_length); // 指定服务端的IP地址。
    servaddr.sin_port = htons(m_port);         // 指定服务端的通信端口。 htons 将主机字节序转换成网络字节序。
    if (::connect(m_clientfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)  // 向服务端发起连接清求。
    { 
      perror("connect"); 
      ::close(m_clientfd); 
      return -1; 
    }


    // 获取 scket 发送缓存区 大小
    int send_buffer_size = 0;
    socklen_t optlen = sizeof(send_buffer_size);
    // 获取发送 缓存区 的大小
    getsockopt(m_clientfd, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, &optlen);
    cout << "send_buffer_size = " << send_buffer_size << endl;

    // 获取 接收 缓存区的 大小
    getsockopt(m_clientfd, SOL_SOCKET, SO_RCVBUF, &send_buffer_size, &optlen);
    cout << "recv_buffer_size = " << send_buffer_size << endl;

    return true;
}


bool ctcpclient::send(const string &buffer){
  if(m_clientfd == -1){
    return false;//如果socket的状态是未连接，直接返回失败。
  }
  if((::send(m_clientfd,buffer.data(),buffer.size(),0))<=0){
    return false;
  
  }
  return true;
}


// 发送 二进制数据 
bool ctcpclient::send( void * buffer, const size_t size){
  if(m_clientfd == -1){
    return false;//如果socket的状态是未连接，直接返回失败。
  }
  if((::send(m_clientfd, buffer, size, 0))<=0){
    return false;
  
  }
  return true;
}

  //接收服务端的报文，成功返回true，失败返回false。
//如果直接操作string对象的内存，必须保证：1)不能越界；2）操作后手动设置数据的大小。
bool ctcpclient::recv(string &buffer,const size_t maxlen){
    buffer.clear();//清空容器。
    buffer.resize(maxlen);//设置容器的大小为maxlen。
    int readn = ::recv(m_clientfd, &buffer[0], buffer.size(), 0);//直接操作buffer的内存。
    if(readn<=0){
        buffer.clear();
        return false;
    }
    buffer.resize(readn);//重置buffer的实际大小。
    return true;
}


bool ctcpclient::close(){
  if(m_clientfd == -1){
    return false;//如果socket的状态是未连接，直接返回失败。
  }
  ::close(m_clientfd);
  m_clientfd = -1;
  return true;
}




 
//  向服务端发送文件 
bool ctcpclient::sendfile(const string &filename, const size_t filesize){
  // 以 二进制的方式 打开文件 ；  ios::binary 是一个标志，表示以二进制模式打开文件
  ifstream fin(filename, ios::binary);
  if(fin.is_open() == false){
    cout << "打开文件 " << filename << "失败 " << endl;

    return false;
  }

  int onread = 0; // 每次调用 fin.read() 时 打算读取的 字节数
  int totalReadBytes = 0; // 已经读取的总字节数
  char buffer[4096];   // 存放读取数据的  buffer 


  while (true)
  {
    memset(buffer, 0, sizeof(buffer)); // 每次读取数据前，将 buffer 清零
    if((filesize - totalReadBytes) > sizeof(buffer)){
      onread = sizeof(buffer);
    } else {
      onread = filesize - totalReadBytes;
    }

    fin.read(buffer, onread);// 从文件中读取数据到 buffer 中, 每次 7个 

    // 把数据发送给 客户端
    if(send(buffer, onread) == false){
      return false;
    }

    // 计算 文件 已读取的字节总数吗如果文件已经读完， 跳出循环
    totalReadBytes += onread;

    if(totalReadBytes >= filesize){
      break;
    }


  }
  






  return true;
};



int client_main(int argc,char *argv[]){

  if(argc!=3)
  {
    cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n"; 
    return-1;
  }
  ctcpclient tcpclient;
  if(tcpclient.connect(argv[1],atoi(argv[2]))==false)//向服务端发起连接请求。
  {
    perror("connect()");
    return-1;
  }







  //第3步：与服务端通讯，客户发送一个请求报文后等待服务端的回复，收到回复后，再发下一个请求报文。
  string buffer;
  for(int ii=0;ii<10;ii++)//循环3次，将与服务端进行三次通讯。
  {
    buffer="这是第"+to_string(ii+1)+"个超级女生，编号"+to_string(ii+1)+"。";
    //向服务端发送请求报文。
    if(tcpclient.send(buffer)==false)
    {
      perror("send");break;
    }
    cout<<"发送："<<buffer<<endl;
    //接收服务端的回应报文，如果服务端没有发送回应报文，recv()函数将阻塞等待。
    if(tcpclient.recv(buffer,1024)==false)
    {
      perror("recv()");break;
    }
    cout<<"接收："<<buffer<<endl;
    sleep(1);
  }

  return 0;
}






