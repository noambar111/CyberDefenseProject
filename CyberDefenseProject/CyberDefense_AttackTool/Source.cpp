#include "AttackCLI.h"
#include <iostream>
#include <WinSock2.h>

int main()
{
    try
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "WSAStartup failed\n";
            return 1;
        }
        AttackCLI cli;
        cli.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}