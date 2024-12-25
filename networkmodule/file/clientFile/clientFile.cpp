#include "clientFile.h"
#include "client.h"







int CLIENT_FILE_MAIN(int argc,char *argv[]){

    // if(argc!=3)
    // {
    //   cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n"; 
    //   return-1;
    // }
    ctcpclient tcpclient;
    if(tcpclient.connect(argv[1],atoi(argv[2]))==false)//向服务端发起连接请求。
    {
      perror("connect()");
      return-1;
    }

      // 发送文件 
      // 1、 把带传输文件名好玩文件的 大小 告诉客户端
      //      定义文件信息的 结构体
      struct st_fileInfo
      {
          char fileName[256]; // 文件名
          int fileSize; // 文件大小
      } fileInfo;

      memset(&fileInfo, 0, sizeof(fileInfo));
      
      strcpy(fileInfo.fileName, argv[3]); // 文件名
      fileInfo.fileSize = atoi(argv[4]); // 文件大小

      //     把文件 信息的 结构体 发送给服务端
      if(tcpclient.send(&fileInfo, sizeof(fileInfo)) == false)
      {
          perror("send");
          return -1;
      }

      cout << "发送文件信息的 结构体 " << fileInfo.fileName << "(" << fileInfo.fileSize << ")" << endl;

      // 2、 等待服务端的确认 报文
      string buffer;
      if( tcpclient.recv( buffer, 3) == false ){
          perror("recv()");
          return -1;
      }
      if(buffer != "ACK"){
          cout << "客户端没有 回应 结构体 "<< endl;
      }
      cout << "客户端 收到  回应 "<< endl;


      // 3、 发送文件内容


      if(tcpclient.sendfile(fileInfo.fileName, fileInfo.fileSize) == false){
            cout << "tcpclient.sendfile log "<< endl;
          perror("sendfile");
          return -1;
      }


      // 4、 等待服务端确认 报文
      if(tcpclient.recv(buffer, 3) == false){
          perror("recv");
          return -1;
      }
      if(buffer != "ACK"){
          cout << "客户端没有 回应 文件内容 "<< endl;
          return -1;
      }


        cout << "发送 文件内容 成功  " << endl;





    return 0;
}


// ./sylar_client 127.0.0.1 8080 ./as.txt 6