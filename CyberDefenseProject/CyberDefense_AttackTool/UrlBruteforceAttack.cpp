#include "UrlBruteforceAttack.h"
#include "HttpClient.h"
#include <chrono>
#include <thread>

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
    using clock = std::chrono::steady_clock;
    
    const int rps = (config.ratePerSecond <= 0) ? 1 : config.ratePerSecond;
    const int duration = (config.durationSeconds <= 0) ? 1 : config.durationSeconds;

    const auto interval = std::chrono::microseconds(1000000 / rps);
    const auto endTime = clock::now() + std::chrono::seconds(duration);
    auto nextTick = clock::now();

    size_t sent = 0;
    size_t ok = 0;
    size_t fail = 0;
    size_t idx = 0;

    std::cout << "[UrlBruteforceAttack] Attacking " << config.host << ":" << config.port << " for" << duration << "s @ " << rps << " RPS\n";

    while (clock::now() < endTime)
    {
        nextTick += interval;
        const std::string& path = m_paths[idx++ % m_paths.size()];

        HttpClient client(config.host, config.port);
        bool success = true;
        if (!client.connectToServer())
        {
            success = false;
        }
        else if (!client.sendGet(path))
        {
            success = false;
        }
        else
        {
            std::string response = client.receiveResponse();
            if (response.empty())
                success = false;
        }

        client.close();

        sent++;
        if (success) ok++;
        else fail++;

        if (sent % 100 == 0)
        {
            std::cout << "sent=" << sent << " ok=" << ok << " fail=" << fail << "\n";
        }

        std::this_thread::sleep_until(nextTick);
    }

    std::cout << "[UrlBruteforceAttack] Finished.\n";
}
