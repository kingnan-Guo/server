#include "clientselect.h"
#include <stdio.h>
#include <client.h>

// int CLIENT_SELECT_MAIN(int argc,char *argv[]){
//     // if(argc!=3)
//     // {
//     //   cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n"; 
//     //   return-1;
//     // }
//     ctcpclient tcpclient;
//     if(tcpclient.connect(argv[1],atoi(argv[2]))==false)//向服务端发起连接请求。
//     {
//       perror("connect()");
//       return-1;
//     }

//     while(1){

//         // char buf[1024]={0};
//         // cin >> buf;
//         // tcpclient.send(buf,strlen(buf));
//         // tcpclient.recv(buf,sizeof(buf));
//         // cout << buf << endl;

//     }

//     return 0;
// }


int CLIENT_SELECT_MAIN(int argc,char *argv[]){

    if(argc!=3)
    {
      cout << "Using:./demo1 服务端的IP 服务端的端口\nExample:./demo1 192.168.101.139 5005\n\n"; 
      return -1;
    }
    ctcpclient tcpclient;
    if(tcpclient.connect(argv[1], atoi(argv[2]))==false)//向服务端发起连接请求。
    {
      perror("connect()");
      return-1;
    }


    char buff[1024];
    string buffer;

    while (1)
    {
      memset(buff, 0, sizeof(buff));
      // printf("请输入要发送的内容：\n");
      cout<<"请输入要发送的内容："<<endl;
      scanf("%s", buff);


      if(tcpclient.send(buff)==false)
      {
        perror("select 客户端 写失败");break;
      }

      // memset(buff, 0, sizeof(buff));
      // if(tcpclient.recv(m_clientfd, buff, sizeof(buff), 0) == -1){
      if(tcpclient.recv(buffer, 1024)==false){
        // cout<<" select 客户端 读失败"<<endl;
        perror("select 客户端 读失败 ");
        // break;
      }
      cout<<"服务端返回的数据为："<< buffer <<endl;
    }


  return 0;
}