// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "clientSynchronization.h"

int clientSynchronization_main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.150.128 5085\n\n"); 
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
 
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))<0) { 
        printf("socket() failed.\n"); 
        return -1; 
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n", argv[1], argv[2]); close(sockfd);  
        return -1;
    }

    printf("connect ok.\n");
    printf("开始时间：%d",time(0));





    // for (int ii=0; ii<100; ii++)
    // {
    //     // 从命令行输入内容。
    //     memset(buf, 0, sizeof(buf));
    //     sprintf(buf, "这是第 %d 个  ", ii);

    //     if (send(sockfd,buf,strlen(buf),0) <=0)       // 把命令行输入的内容发送给服务端。
    //     { 
    //         printf("write() failed.\n");
    //         close(sockfd);
    //         return -1;
    //     }
        
    //     memset(buf,0,sizeof(buf));
    //     if (recv(sockfd,buf,sizeof(buf),0) <=0)      // 接收服务端的回应。
    //     { 
    //         printf("read() failed.\n");
    //         close(sockfd);
    //         return -1;
    //     }

    //     printf("recv:%s\n",buf);
    // }





    //  有报头的 方式。
    for (int ii=0; ii<4; ii++)
    {
        // 从命令行输入内容。


        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是第%d个 数据。",ii);

        int len=strlen(buf);        // 得到报文长度。
        char tmp[1024];             // 报文头部（4字节的整数）+ 报文内容
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, &len, 4);
        memcpy(tmp+4, buf, len);

        if (send(sockfd,tmp,len+4,0) <=0)       // 把整个报文的内容发送给服务端。
        { 
            printf("write() failed.\n");  
            close(sockfd);  
            return -1;
        }
        // send(sockfd,tmp,len+4,0);

    }

    for (int ii=0; ii<4; ii++)
    {
        
        // memset(buf,0,sizeof(buf));
        // if (recv(sockfd,buf,sizeof(buf),0) <=0)      // 接收服务端的回应。
        // { 
        //     printf("read() failed.\n");
        //     close(sockfd);
        //     return -1;
        // }

        // printf("recv:%s\n",buf);

        int len;
        recv(sockfd,&len,4,0);            // 先读取4字节的报文头部。

        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,len,0);           // 读取报文内容。

        printf("recv:%s\n",buf);

    }




    //  没有报头的 方式。

    /*

    for (int ii=0; ii<4; ii++)
    {
        // 从命令行输入内容。


        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是第%d个 数据。",ii);

        int len=strlen(buf);        // 得到报文长度。

        if (send(sockfd,buf,len,0) <=0)       // 把整个报文的内容发送给服务端。
        { 
            printf("write() failed.\n");  
            close(sockfd);  
            return -1;
        }
        // send(sockfd,tmp,len+4,0);
    }
    */

    // printf("结束时间：%d",time(0));
} 