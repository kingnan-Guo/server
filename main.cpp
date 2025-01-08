#include <iostream>
#include <stdio.h>
#include "SQLITE.h"
#include "TCPEPOLL.h"



//  ./Reactor 172.27.124.240  8080
int main(int arch, char* argv[])
{
    printf("Reactor star \r\n");
    TCPEPOLL(arch, argv);  // EchoServer DEMO
    // SQLITE_MAIN();

    return 0;
}



