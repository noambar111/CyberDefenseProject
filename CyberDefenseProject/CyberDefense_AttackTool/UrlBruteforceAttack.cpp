#include "UrlBruteforceAttack.h"

UrlBruteforceAttack::UrlBruteforceAttack()
{
    m_paths = {
        "/geo?ip=1.1.1.1",
        "/geo?ip=8.8.8.8",
        "/top",
        "/ips?country=USA",
        "/insert?ip=5.5.5.5&country=USA"
    };
}

void UrlBruteforceAttack::run(const AttackConfig& config)
{
    std::cout << "[UrlBruteforceAttack] Attacking " << config.host << ":" << config.port << "\n";
    for (const auto& path : m_paths)
    {
        std::cout << "Would send GET " << path << std::endl;
    }
    std::cout << "[UrlBruteforceAttack] Finished.\n";
}
