#include "serverFile.h"

#include "networkMultiProcess.h"


// 初始化服务端用于监听的socket。
ctcpserverMultiProcess serverFile_tcpserver;


void SERVER_FILE_ParentEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN); 
    std::cout << "SERVER_FILE_ParentEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;
    kill(0, SIGTERM); // 向所有子进程发送 SIGTERM 15 信号,通知全部的子进程 退出; kill 0  表示向所有子进程发送信号 也会发给自己
    serverFile_tcpserver.closelisten(); // 关闭监听的socket
    // 释放资源

    exit(0);
};

void SERVER_FILE_ChildEXIT(int sig){
    // 防止信号 处理在执行过程中再次被信号中断
    signal (SIGINT, SIG_IGN);
    signal (SIGTERM, SIG_IGN);
    std::cout << "SERVER_FILE_ChildEXIT  进程 "<<  getpid() << " sig ==" << sig << std::endl;
    serverFile_tcpserver.closeclient(); // 关闭客户端的socket
    // 释放资源 只释放子进程
    exit(0);
};






int SERVER_FILE_MAIN(int argc,char *argv[]){

    if (argc!=3)
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

    signal (SIGTERM,  SERVER_FILE_ParentEXIT); // 15  终止进程
    signal (SIGINT,   SERVER_FILE_ParentEXIT); //  2  中断进程



    // ================================================
    // 初始化服务端用于监听的socket。
    // ctcpserverMultiProcess serverFile_tcpserver;
    if (serverFile_tcpserver.initserver(atoi(argv[1])) == false) // 初始化服务端用于监听的socket。
    {
      perror("initserver()"); 
      return -1;
    }

    cout << "initserver ok\n" << endl;


    while (true)
    {


        // 受理客户端的连接（从已连接的客户端中取出一个客户端），  
        // 如果没有已连接的客户端，accept()函数将阻塞等待。
        if (serverFile_tcpserver.accept() == false)
        {
          perror("accept()"); 
          return -1;
        }
        cout << "客户端已连接(" << serverFile_tcpserver.clientip() << ")。\n";



        int pid = fork();  // 创建子进程。
        if (pid == -1){ // 创建子进程失败。
          perror("fork()"); 
          return -1;
        }
        // 父进程 , 父进程返回循环开始的位置，继续受理 客户端 的连接。
        if (pid > 0){
          
          // 父进程 关闭 与 客户端 的连接的socket。
          serverFile_tcpserver.closeclient(); // 关闭客户端的socket。
          continue;
        }

        // 子进程，pid 返回0。
        if(pid == 0){
            cout << " 子进程 (" << serverFile_tcpserver.clientip() << ")。\n";


            serverFile_tcpserver.closelisten(); //

            signal (SIGTERM, SERVER_FILE_ChildEXIT); // 子进程的 退出函数 与 父进程 不一样
            signal (SIGINT, SIG_IGN);// 子进程 不需要 捕获 SIGINT 信号, 忽略
        }
        // 子进程 负责 与客户端进行 通信 

        cout << " 客户端已连接 (" << serverFile_tcpserver.clientip() << ")。\n";




        // ============================================================



        // 1、 接收文件名和文件大小信息
        //        定义文件信息的结构体
        struct st_fileInfo
        {
            char fileName[256];   // 文件名
            int fileSize;         // 文件大小
        } fileInfo;

        memset(&fileInfo, 0, sizeof(fileInfo));
        //        用结构体 接收报文的内容
        if( serverFile_tcpserver.recv(&fileInfo, sizeof(fileInfo) ) == false ){
            perror("recv()");
            return -1;
        }

        cout << "接收文件信息的 结构体 " << fileInfo.fileName << "(" << fileInfo.fileSize << ")" << endl;


        // 2、给客户端恢复确认报文，表示客户端可以发送文件了
        if(serverFile_tcpserver.send("ACK") == false) {
            perror("send()");
            return -1;
        }


        // 3、接收文件数据，并保存到本地文件中
        //. argv[2]  char *  转换为 string;  "/"是。const char * ，fileInfo.fileName 是 char *
        if(serverFile_tcpserver.recvFile(  (string) argv[2] + "/" + fileInfo.fileName, fileInfo.fileSize) == false) {
            // perror("recvFile()");
            cout << "接收 文件内容 失败  " << endl;
            return -1;

        }

        cout << "接收 文件内容 成功  " << endl;


        // 4、给客户端恢复确认报文，表示文件接收完毕，客户端可以关闭连接了

        if(serverFile_tcpserver.send("ACK") == false) {
            
        }


        return 0; // 子进程 一定要 退出 ，否则 又会回到 accept 函数的 位置

    }
    


}





// ./sylar_servers 8080 /tmp 