#include "UrlBruteforceAttack.h"
#include "HttpClient.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>
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
    auto tStart = clock::now();
    const auto endTime = tStart + std::chrono::seconds(duration);
    auto nextTick = clock::now();

    size_t threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 8;

    AttackThreadPool pool(threadCount);

    std::atomic<size_t> scheduled{ 0 };
    std::atomic<size_t> sent{ 0 }, ok{ 0 }, fail{ 0 };

    std::atomic<size_t> connectOk{ 0 }, connectFail{ 0 };
    std::atomic<size_t> sendOk{ 0 }, sendFail{ 0 };
    std::atomic<size_t> recvOk{ 0 }, recvFail{ 0 };

    std::atomic<long long> totalReqLatencyMs{ 0 };
    std::atomic<long long> totalConnectLatencyMs{ 0 };
    std::atomic<long long> totalSendLatencyMs{ 0 };
    std::atomic<long long> totalRecvLatencyMs{ 0 };

    std::atomic<size_t> pending{ 0 };
    std::mutex doneMtx;
    std::condition_variable doneCv;

    size_t idx = 0;

    auto toMs = [](auto d) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    };

    std::cout << "[UrlBruteforceAttack] START at t=0ms"
        << " duration=" << duration << "s"
        << " target=" << config.host << ":" << config.port
        << " rps=" << rps << "\n";

    while (clock::now() < endTime)
    {
        nextTick += interval;
        const std::string path = m_paths[idx++ % m_paths.size()];

        scheduled.fetch_add(1, std::memory_order_relaxed);
        pending.fetch_add(1, std::memory_order_relaxed);

        pool.enqueue([&, path]() {
            bool success = true;

        auto reqStart = clock::now();

        HttpClient client(config.host, config.port);

        auto connectStart = clock::now();
        if (!client.connectToServer())
        {
            success = false;
            connectFail.fetch_add(1, std::memory_order_relaxed);
        }
        else
        {
            auto connectEnd = clock::now();
            connectOk.fetch_add(1, std::memory_order_relaxed);
            totalConnectLatencyMs.fetch_add(
                std::chrono::duration_cast<std::chrono::milliseconds>(connectEnd - connectStart).count(),
                std::memory_order_relaxed
            );

            auto sendStart = clock::now();
            if (!client.sendGet(path))
            {
                success = false;
                sendFail.fetch_add(1, std::memory_order_relaxed);
            }
            else
            {
                auto sendEnd = clock::now();
                sendOk.fetch_add(1, std::memory_order_relaxed);
                totalSendLatencyMs.fetch_add(
                    std::chrono::duration_cast<std::chrono::milliseconds>(sendEnd - sendStart).count(),
                    std::memory_order_relaxed
                );

                auto recvStart = clock::now();
                std::string response = client.receiveResponse();
                auto recvEnd = clock::now();

                if (response.empty())
                {
                    success = false;
                    recvFail.fetch_add(1, std::memory_order_relaxed);
                }
                else
                {
                    recvOk.fetch_add(1, std::memory_order_relaxed);
                    totalRecvLatencyMs.fetch_add(
                        std::chrono::duration_cast<std::chrono::milliseconds>(recvEnd - recvStart).count(),
                        std::memory_order_relaxed
                    );
                }
            }
        }

        client.close();

        auto reqEnd = clock::now();
        totalReqLatencyMs.fetch_add(
            std::chrono::duration_cast<std::chrono::milliseconds>(reqEnd - reqStart).count(),
            std::memory_order_relaxed
        );

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

    auto tScheduleEnd = clock::now();
    std::cout << "[UrlBruteforceAttack] SCHEDULING ENDED at "
        << toMs(tScheduleEnd - tStart) << "ms"
        << " (expected ~" << (duration * 1000) << "ms)"
        << " pending=" << pending.load() << "\n";

    {
        std::unique_lock<std::mutex> ul(doneMtx);
        doneCv.wait(ul, [&] {
            return pending.load(std::memory_order_relaxed) == 0;
            });
    }

    auto tDone = clock::now();

    const auto totalDone = sent.load();
    const auto scheduledCount = scheduled.load();

    const auto connectOkCount = connectOk.load();
    const auto sendOkCount = sendOk.load();
    const auto recvOkCount = recvOk.load();

    double avgReqMs = (totalDone > 0)
        ? static_cast<double>(totalReqLatencyMs.load()) / totalDone
        : 0.0;

    double avgConnectMs = (connectOkCount > 0)
        ? static_cast<double>(totalConnectLatencyMs.load()) / connectOkCount
        : 0.0;

    double avgSendMs = (sendOkCount > 0)
        ? static_cast<double>(totalSendLatencyMs.load()) / sendOkCount
        : 0.0;

    double avgRecvMs = (recvOkCount > 0)
        ? static_cast<double>(totalRecvLatencyMs.load()) / recvOkCount
        : 0.0;

    std::cout << "[UrlBruteforceAttack] DONE at "
        << toMs(tDone - tStart) << "ms"
        << " scheduled=" << scheduledCount
        << " totalDone=" << totalDone
        << " ok=" << ok.load()
        << " fail=" << fail.load()
        << " pending=" << pending.load() << "\n";

    std::cout << "[UrlBruteforceAttack] STATS | "
        << "connectOk=" << connectOk.load()
        << " connectFail=" << connectFail.load()
        << " sendOk=" << sendOk.load()
        << " sendFail=" << sendFail.load()
        << " recvOk=" << recvOk.load()
        << " recvFail=" << recvFail.load()
        << "\n";

    std::cout << "[UrlBruteforceAttack] AVG LATENCY | "
        << "req=" << avgReqMs << "ms "
        << "connect=" << avgConnectMs << "ms "
        << "send=" << avgSendMs << "ms "
        << "recv=" << avgRecvMs << "ms"
        << "\n";

    std::cout << "[UrlBruteforceAttack] Finished.\n";
}