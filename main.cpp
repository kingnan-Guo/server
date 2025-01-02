#include <iostream>
#include <stdio.h>
#include "client.h"
#include "clientSynchronization.h"


int main(int arch, char* argv[])
{
    printf("ReactorClient star \r\n");

    // client_main(arch, argv);
    clientSynchronization_main(arch, argv);
    return 0;
}
