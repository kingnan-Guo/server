/*
 * 程序名：demo2.cpp，此程序用于演示socket通信的服务端
*/
#include "networkMultiProcess.h"


using namespace std;
 




bool ctcpserverMultiProcess::initserver(const unsigned short in_port){


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


    int opt = 1; //SO_REUSEADDR允许重用本地地址和端口
    if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        return false;
    }



    // 关闭 Nagle 算法 ； Nagle 算法的作用是尽量发送大块数据，减少网络交互次数，但会造成一定的延迟。大概 40ms 左右。
    // int flag = 1;
    // if (setsockopt(m_listenfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
    //     perror("setsockopt(TCP_NODELAY) failed");
    //     return false;
    // }

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


// bool ctcpserverMultiProcess::accept()
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
bool ctcpserverMultiProcess::accept(){
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




bool ctcpserverMultiProcess::send(const string &buffer){
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
bool ctcpserverMultiProcess::recv(string &buffer,const size_t maxlen)
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



bool ctcpserverMultiProcess::recv(void * buffer,const size_t size)
{ 
    if ( (::recv(m_clientfd, buffer, size, 0)) <= 0 ){
        return false;
    }
    return true;
}



bool ctcpserverMultiProcess::recvFile(const string &filename, const size_t filesize)
{ 
    ofstream fout;
    fout.open(filename, ios::binary); // 二进制形式 打开文件 
    if(fout.is_open() == false){
      cout << "打开文件 " << filename << "失败 " << endl;
      return false;
    }

    int totalReadBytes = 0; // 已经 接收 的总字节数
    int onread = 0; //   打算接收的 字节数
    char buffer[4096];   //  接收 文件内容的缓冲区

    while (true)
    {
      if((filesize - totalReadBytes) > sizeof(buffer)){
        onread = sizeof(buffer);
      } else {
        onread = filesize - totalReadBytes;
      }


      // 接收文件 内容 
      if(recv(buffer, onread) == false){
        return false;
      }

      // 把接收到的内容 写入文件
      fout.write(buffer, onread);


      // 计算 文件 已读取的字节总数吗如果文件已经读完， 跳出循环
      totalReadBytes += onread;

      if(totalReadBytes >= filesize){
        break;
      }

    }



    return true;
}








// ============================================



// 初始化服务端用于监听的socket。
ctcpserverMultiProcess tcpserver;



void NETWORK_MULTI_PROCESS_ParentEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN); 
    std::cout << "NETWORK_MULTI_PROCESS_ParentEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;
    kill(0, SIGTERM); // 向所有子进程发送 SIGTERM 15 信号,通知全部的子进程 退出; kill 0  表示向所有子进程发送信号 也会发给自己
    tcpserver.closelisten(); // 关闭监听的socket
    // 释放资源

    exit(0);
};

void NETWORK_MULTI_PROCESS_ChildEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN);
    std::cout << "NETWORK_MULTI_PROCESS_ChildEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;
    tcpserver.closeclient(); // 关闭客户端的socket
    // 释放资源 只释放子进程
    exit(0);
};






int NETWORK_MULTI_PROCESS_MAIN(int argc,char *argv[]){

    if (argc!=2)
    {
        cout << "Using:./demo2 通讯端口\n Example:./demo2 5005 \n\n";   // 端口大于1024，不与其它的重复。
        cout << "注意: 运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
        cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
        return -1;
    }


    // 忽略全部 64 个信号
    for (size_t i = 1; i < 64; i++)
    {
        signal(i, SIG_IGN); // 忽略信号
    }

    signal (SIGTERM,  NETWORK_MULTI_PROCESS_ParentEXIT); // 15  终止进程
    signal (SIGINT,   NETWORK_MULTI_PROCESS_ParentEXIT); //  2  中断进程



    // ================================================
    // 初始化服务端用于监听的socket。
    // ctcpserverMultiProcess tcpserver;
    if (tcpserver.initserver(atoi(argv[1])) == false) // 初始化服务端用于监听的socket。
    {
      perror("initserver()"); 
      return -1;
    }

    cout << "initserver ok\n" << endl;


    while (true)
    {


        // 受理客户端的连接（从已连接的客户端中取出一个客户端），  
        // 如果没有已连接的客户端，accept()函数将阻塞等待。
        if (tcpserver.accept() == false)
        {
          perror("accept()"); 
          return -1;
        }
        cout << "客户端已连接(" << tcpserver.clientip() << ")。\n";



        int pid = fork();  // 创建子进程。
        if (pid == -1){ // 创建子进程失败。
          perror("fork()"); 
          return -1;
        }
        // 父进程 , 父进程返回循环开始的位置，继续受理 客户端 的连接。
        if (pid > 0){
          
          // 父进程 关闭 与 客户端 的连接的socket。
          tcpserver.closeclient(); // 关闭客户端的socket。
          continue;
        }

        // 子进程，pid 返回0。
        if(pid == 0){
            cout << " 子进程 (" << tcpserver.clientip() << ")。\n";


            tcpserver.closelisten(); //

            signal (SIGTERM, NETWORK_MULTI_PROCESS_ChildEXIT); // 子进程的 退出函数 与 父进程 不一样
            signal (SIGINT, SIG_IGN);// 子进程 不需要 捕获 SIGINT 信号, 忽略
        }


        string buffer; // 存放接收到的报文的内容。
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

        return 0; // 子进程 一定要 退出 ，否则 又会回到 accept 函数的 位置


    }
    


}


// 服务端程序 fork 之后 会把监听的socker 和 客户端连上来的 socker 都赋值到 子进程中，
// 在多进程的 服务端程序中， 父进程 只负责 客户端连接 ， 子进程 负责 与客户端 通信