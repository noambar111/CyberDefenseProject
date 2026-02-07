#include "UrlBruteforceAttack.h"
#include "HttpClient.h"
#include <chrono>
#include <thread>
#include "AttackThreadPool.h"

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

    size_t threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 8;

    AttackThreadPool pool(threadCount);

    std::atomic<size_t> sent{ 0 }, ok{ 0 }, fail{ 0 };

    std::atomic<size_t> pending{ 0 };
    std::mutex doneMtx;
    std::condition_variable doneCv;


    size_t idx = 0;

    std::cout << "[UrlBruteforceAttack] Attacking " << config.host << ":" << config.port << " for" << duration << "s @ " << rps << " RPS\n";

    while (clock::now() < endTime)
    {
        nextTick += interval;
        const std::string path = m_paths[idx++ % m_paths.size()];
        pending.fetch_add(1, std::memory_order_relaxed);
        pool.enqueue([&, path]() {
            bool success = true;

        HttpClient client(config.host, config.port);
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
            if (response.empty()) success = false;
        }

        client.close();

        sent.fetch_add(1, std::memory_order_relaxed);
        if (success) ok.fetch_add(1, std::memory_order_relaxed);
        else fail.fetch_add(1, std::memory_order_relaxed);

        if (pending.fetch_sub(1, std::memory_order_relaxed) == 1)
        {
            std::lock_guard<std::mutex> lg(doneMtx);
            doneCv.notify_one();
        }

            });
        std::this_thread::sleep_until(nextTick);
    }

    {
        std::unique_lock<std::mutex> ul(doneMtx);
        doneCv.wait(ul, [&] {
            return pending.load(std::memory_order_relaxed) == 0;
            });
    }

    std::cout << "[UrlBruteforceAttack] Finished.\n";
}
