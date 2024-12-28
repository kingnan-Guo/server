#include <iostream>
#include <stdio.h>
#include "tcpepoll3.h"



int main(int arch, char* argv[])
{
    printf("Reactor star \r\n");

    tcpepoll3(arch, argv);
    return 0;
}
