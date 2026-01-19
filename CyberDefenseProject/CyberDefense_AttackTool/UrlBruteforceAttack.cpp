#include "UrlBruteforceAttack.h"
#include "HttpClient.h"

UrlBruteforceAttack::UrlBruteforceAttack()
{
    m_paths = {
        "/geo?ip=1.1.1.1",
        "/geo?ip=8.8.8.8",
        "/top_countries",
        "/countries_to_ip?country=USA",
        "/add_rec?ip=5.5.5.5&country=USA"
    };
}

void UrlBruteforceAttack::run(const AttackConfig& config)
{
    std::cout << "[UrlBruteforceAttack] Attacking " << config.host << ":" << config.port << "\n";

    for (const auto& path : m_paths)
    {
        HttpClient client(config.host, config.port);

        if (!client.connectToServer())
        {
            std::cerr << "Failed to connect to server\n";
            return;
        }

        std::cout << "sending GET " << path << std::endl;
        client.sendGet(path);
        std::string response = client.receiveResponse();
        std::cout << "Response:\n" << response << "\n";

        client.close();
    }
    std::cout << "[UrlBruteforceAttack] Finished.\n";
}
