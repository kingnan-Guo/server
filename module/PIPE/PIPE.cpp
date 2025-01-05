#include "PIPE.h"

void PIPE_TEST(int argc,char *argv[]){
    int pipeFd[2];// 声明 无名管道。 有两个 fd 表示管道的两端， fd[0] 表示读端，fd[1] 表示写端

    //创建管道
    if(pipe2(pipeFd, 0) == -1){ // 创建 无名 管道， O_CLOEXEC | O_NONBLOCK ： 非阻塞型 ; 0 
        printf("pipe2 error\n");
        return;
    }

    /**
     * 
     * params
     *  fd : 管道的一端
     * buf : 写入的数据, 数据的地址
     * count : 写入的数据长度
     */
    write(pipeFd[1], "hellow world", 12);// 往 管道咯写入数据与

    ssize_t ret;

    char buf[128] = {0};

    /**
     * params
     *  fd : 管道的一端
     * buf : 读取的数据，数据的地址
     * count : 读取的数据长度
     */
    // ret = read(pipeFd[0], &buf, sizeof(buf));//     从管道读取数据
    ret = read(pipeFd[0], &buf, 6);// 
    printf("ret = %d  buf =  %s\n", ret, buf);


    ret = read(pipeFd[0], &buf, sizeof(buf)); //    从管道读取数据 ，但是 会阻塞，因为管道中没有数据
    printf("ret = %d  buf =  %s\n", ret, buf);

    
    close(pipeFd[0]);
    close(pipeFd[1]);

}

int PIPE_MAIN(int argc,char *argv[])
{
    PIPE_TEST(argc, argv);
    return 0;
}
