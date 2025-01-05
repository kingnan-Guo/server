#include "EVENTFD.h"

void EVENTFD_ONE(int argc,char *argv[]){
    /**
     * eventfd 
     * params
     *      第一个参数： 默认值； 如果 为 0 ，则表示 eventfd 是一个计数器，否则是一个信号量； 如果是 2 那么会与 传入的 buf 相加
     *      第二个参数 ： 
     *              EFD_CLOCKED ，表示 eventfd 是一个计数器，否则是一个信号量；
     *              EFD_SEMAPHORE ， 信号量语义;  EFD_NONBLOCK | EFD_SEMAPHORE 每次获取到的 都是 1 ， 如果 值是 5 ，那么在 获取到第六次的时候ret 为 -1 ；  如果把 阻塞的 EFD_NONBLOCK 去掉就变成了信号量， 前面5次都成功，第六次 阻塞
     *              EFD_NONBLOCK ，表示 eventfd 是一个计数器，否则是一个信号量； 非阻塞
     * 
     * 只要是 fd 就可以加入 epoll
     */
    int efd = eventfd(2, EFD_NONBLOCK | EFD_SEMAPHORE);
    uint64_t buf = 3;
    ssize_t ret;

    //写 eventfd ， buf 必须是 8 字节；
    ret = write(efd, &buf, sizeof(uint64_t));

    // 从eventfd    读数据，buf 必须是 8 字节；
    ret = read(efd, &buf, sizeof(uint64_t));
    printf("1 ret = %ld buf =  %ld \n", ret, buf);

    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("2 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞

    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("3 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞

    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("4 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞

    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("5 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞

    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("6 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞
    
    ret = read(efd, &buf, sizeof(uint64_t));// ret 会返回 -1
    printf("6 ret = %ld buf =  %ld \n", ret, buf);// 再次打印被 阻塞了， 因为 eventfd 中的数据已经被读完了， 所以 buf 是 0。所以阻塞

    close(efd);

}

int EVENTFD_MAIN(int argc,char *argv[])
{
    EVENTFD_ONE(argc, argv);
    return 0;
}
