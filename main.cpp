#include <iostream>
#include <stdio.h>
// #include "tcpepoll3.h"
// #include "tcpepoll4.h"
#include "ThreadPool.h"



int main(int arch, char* argv[])
{
    printf("Reactor star \r\n");

    // tcpepoll3(arch, argv);
    // tcpepoll4(arch, argv);


    ThreadPool_MAIN();
    return 0;
}
