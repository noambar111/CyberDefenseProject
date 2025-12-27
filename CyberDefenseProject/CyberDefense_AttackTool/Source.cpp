#include "AttackCLI.h"
#include <iostream>

int main()
{
    try
    {
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