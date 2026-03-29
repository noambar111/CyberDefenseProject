#include "Server.h"
#include <iostream>
#include<string>
#include "Response.h"
#include <map>

#pragma comment(lib, "ws2_32.lib")
#define LEN 4096

void Server::start()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        std::cerr << "WSAStartup failed. Error: " << result << std::endl;
        return;
    }

    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(m_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return;
    }

    result = listen(m_serverSocket, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Listen failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server is running on port " << m_port << "..." << std::endl;
    Logger::getInstance().log(LogLevel::INFO_LOG, "Server starting on port " + std::to_string(m_port));

    std::thread([this]()
        {
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));

                long long accepted = m_totalAccepted.exchange(0, std::memory_order_relaxed);
                long long enqueued = m_totalEnqueued.exchange(0, std::memory_order_relaxed);
                long long started = m_totalStarted.exchange(0, std::memory_order_relaxed);
                long long completed = m_totalCompleted.exchange(0, std::memory_order_relaxed);

                long long queueUs = m_totalQueueTimeUs.exchange(0, std::memory_order_relaxed);
                long long recvUs = m_totalRecvTimeUs.exchange(0, std::memory_order_relaxed);
                long long parseUs = m_totalParseTimeUs.exchange(0, std::memory_order_relaxed);
                long long routeUs = m_totalRouteTimeUs.exchange(0, std::memory_order_relaxed);
                long long serializeUs = m_totalSerializeTimeUs.exchange(0, std::memory_order_relaxed);
                long long sendUs = m_totalSendTimeUs.exchange(0, std::memory_order_relaxed);
                long long processingUs = m_totalProcessingTimeUs.exchange(0, std::memory_order_relaxed);
                long long totalUs = m_totalTotalTimeUs.exchange(0, std::memory_order_relaxed);

                long long activeNow = m_activeWorkers.load(std::memory_order_relaxed);
                size_t poolQueueNow = m_pool.getQueueSize();

                std::cout << "\n=== SERVER STATS (last 2s) ===\n";
                std::cout << "accepted=" << accepted << "\n";
                std::cout << "enqueued=" << enqueued << "\n";
                std::cout << "started=" << started << "\n";
                std::cout << "completed=" << completed << "\n";
                std::cout << "activeWorkers(now)=" << activeNow << "\n";
                std::cout << "poolQueueSize(now)=" << poolQueueNow << "\n";

                if (completed > 0)
                {
                    std::cout << "avgQueue=" << (queueUs / completed) << " us\n";
                    std::cout << "avgRecv=" << (recvUs / completed) << " us\n";
                    std::cout << "avgParse=" << (parseUs / completed) << " us\n";
                    std::cout << "avgRoute=" << (routeUs / completed) << " us\n";
                    std::cout << "avgSerialize=" << (serializeUs / completed) << " us\n";
                    std::cout << "avgSend=" << (sendUs / completed) << " us\n";
                    std::cout << "avgProcessing=" << (processingUs / completed) << " us\n";
                    std::cout << "avgTotal=" << (totalUs / completed) << " us\n";
                }
                else
                {
                    std::cout << "No completed requests in this window.\n";
                }

                std::cout << "==============================\n";
            }
        }).detach();

        while (true)
        {
            SOCKET client = accept(m_serverSocket, nullptr, nullptr);
            if (client == INVALID_SOCKET)
            {
                std::cerr << "Accept failed. Error: " << WSAGetLastError() << std::endl;
                continue;
            }

            auto acceptedAt = std::chrono::steady_clock::now();
            m_totalAccepted.fetch_add(1, std::memory_order_relaxed);
            m_totalEnqueued.fetch_add(1, std::memory_order_relaxed);

            m_pool.enqueue([this, client, acceptedAt]()
                {
                    this->handleClient(client, acceptedAt);
                });
        }

        closesocket(m_serverSocket);
        WSACleanup();
}

void Server::handleClient(int clientSocket, std::chrono::steady_clock::time_point acceptedAt)
{
    using clock = std::chrono::steady_clock;

    m_totalStarted.fetch_add(1, std::memory_order_relaxed);
    m_activeWorkers.fetch_add(1, std::memory_order_relaxed);

    auto workerStart = clock::now();
    auto queueTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(workerStart - acceptedAt).count();

    char buf[LEN] = { 0 };

    auto recvStart = clock::now();
    int bytesReceived = recv(clientSocket, buf, LEN, 0);
    auto recvEnd = clock::now();

    auto recvTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(recvEnd - recvStart).count();

    if (bytesReceived <= 0)
    {
        closesocket(clientSocket);
        m_activeWorkers.fetch_sub(1, std::memory_order_relaxed);
        return;
    }

    auto parseStart = clock::now();
    Request req(std::string(buf, bytesReceived));
    auto parseEnd = clock::now();

    auto parseTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(parseEnd - parseStart).count();

    auto routeStart = clock::now();
    Response res = m_router.route(req);
    auto routeEnd = clock::now();

    auto routeTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(routeEnd - routeStart).count();

    auto serializeStart = clock::now();
    std::string responseStr = res.toString();
    auto serializeEnd = clock::now();

    auto serializeTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(serializeEnd - serializeStart).count();

    auto sendStart = clock::now();
    int sentBytes = send(clientSocket, responseStr.c_str(), static_cast<int>(responseStr.size()), 0);
    auto sendEnd = clock::now();

    auto sendTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(sendEnd - sendStart).count();

    auto finishedAt = clock::now();

    auto processingTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(finishedAt - workerStart).count();

    auto totalTimeUs =
        std::chrono::duration_cast<std::chrono::microseconds>(finishedAt - acceptedAt).count();

    m_totalQueueTimeUs.fetch_add(queueTimeUs, std::memory_order_relaxed);
    m_totalRecvTimeUs.fetch_add(recvTimeUs, std::memory_order_relaxed);
    m_totalParseTimeUs.fetch_add(parseTimeUs, std::memory_order_relaxed);
    m_totalRouteTimeUs.fetch_add(routeTimeUs, std::memory_order_relaxed);
    m_totalSerializeTimeUs.fetch_add(serializeTimeUs, std::memory_order_relaxed);
    m_totalSendTimeUs.fetch_add(sendTimeUs, std::memory_order_relaxed);
    m_totalProcessingTimeUs.fetch_add(processingTimeUs, std::memory_order_relaxed);
    m_totalTotalTimeUs.fetch_add(totalTimeUs, std::memory_order_relaxed);

    m_totalCompleted.fetch_add(1, std::memory_order_relaxed);

    if (sentBytes == SOCKET_ERROR)
    {
    }

    closesocket(clientSocket);
    m_activeWorkers.fetch_sub(1, std::memory_order_relaxed);
}
