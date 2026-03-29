#include "AttackCLI.h"
#include <iostream>
#include "UrlBruteforceAttack.h"

void AttackCLI::printMenu() const
{
    std::cout << "=== Attack Service ===\n";
    std::cout << "1) URL brute-force attack\n";
    std::cout << "Choose attack type: ";
}

int AttackCLI::readChoice() const
{
    int c = 0;
    std::cin >> c;
    return c;
}

AttackConfig AttackCLI::readConfig() const
{
    AttackConfig cfg;
    std::cout << "Target host (e.g. 127.0.0.1): ";
    std::cin >> cfg.host;

    std::cout << "Target port (e.g. 8080): ";
    std::cin >> cfg.port;

    std::cout << "Duration in seconds: ";
    std::cin >> cfg.durationSeconds;

    std::cout << "Request per seconds (RPS): ";
    std::cin >> cfg.ratePerSecond;

    return cfg;
}

std::unique_ptr<IAttack> AttackCLI::createAttack(int choice) const
{
    switch (choice)
    {
    case 1:
        return std::make_unique<UrlBruteforceAttack>();
    default:
        return nullptr;
    }

}

void AttackCLI::run()
{
    printMenu();
    int c = readChoice();
    auto attack = createAttack(c);
    if (!attack)
    {
        std::cout << "Invalid choice.\n";
        return;
    }

    AttackConfig cfg = readConfig();
    std::cout << "Running attack: " << attack->name() << "\n";
    attack->run(cfg);
    std::cout << "Attack finished.\n";


}
