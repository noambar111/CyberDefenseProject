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

    std::thread([this]() {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));

            size_t req = m_totalRequests.load(std::memory_order_relaxed);
            if (req == 0) continue;

            long long queue = m_totalQueueTimeMs.load(std::memory_order_relaxed);
            long long route = m_totalRouteTimeMs.load(std::memory_order_relaxed);
            long long processing = m_totalProcessingTimeMs.load(std::memory_order_relaxed);
            long long total = m_totalTotalTimeMs.load(std::memory_order_relaxed);
            long long recv = m_totalRecvTimeMs.exchange(0, std::memory_order_relaxed);
            long long parse = m_totalParseTimeMs.exchange(0, std::memory_order_relaxed);
            long long rroute = m_totalRouteTimeMs.exchange(0, std::memory_order_relaxed);
            long long serialize = m_totalSerializeTimeMs.exchange(0, std::memory_order_relaxed);
            long long send = m_totalSendTimeMs.exchange(0, std::memory_order_relaxed);

            std::cout << "\n=== SERVER STATS (last 2s) ===\n";
            std::cout << "requests=" << req << "\n";
            std::cout << "avgQueue=" << (queue / req) << " ms\n";
            std::cout << "avgRecv=" << (recv / req) << " ms\n";
            std::cout << "avgParse=" << (parse / req) << " ms\n";
            std::cout << "avgRoute=" << (rroute / req) << " ms\n";
            std::cout << "avgSerialize=" << (serialize / req) << " ms\n";
            std::cout << "avgSend=" << (send / req) << " ms\n";
            std::cout << "avgProcessing=" << (processing / req) << " ms\n";
            std::cout << "avgTotal=" << (total / req) << " ms\n";
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
        else
        {
            auto acceptedAt = std::chrono::steady_clock::now();

            m_pool.enqueue([this, client, acceptedAt]()
                {
                    this->handleClient(client, acceptedAt);
                });
            //std::thread([this, client]() {
            //    this->handleClient(client);
            //closesocket(client);
            //    }).detach();
        }
    }

    closesocket(m_serverSocket);
    WSACleanup();
}

void Server::handleClient(int clientSocket, std::chrono::steady_clock::time_point acceptedAt)
{
    using clock = std::chrono::steady_clock;

    auto workerStart = clock::now();
    auto queueTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(workerStart - acceptedAt).count();

    char buf[LEN] = { 0 };

    auto recvStart = clock::now();
    int bytesReceived = recv(clientSocket, buf, LEN, 0);
    auto recvEnd = clock::now();

    auto recvTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(recvEnd - recvStart).count();

    if (bytesReceived <= 0)
    {
        //Logger::getInstance().log(
        //    LogLevel::ERROR_LOG,
        //    "recv failed or client disconnected"
        //);
        closesocket(clientSocket);
        return;
    }

    auto parseStart = clock::now();
    Request req(std::string(buf, bytesReceived));
    auto parseEnd = clock::now();

    auto parseTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(parseEnd - parseStart).count();

    auto routeStart = clock::now();
    Response res = m_router.route(req);
    auto routeEnd = clock::now();

    auto routeTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(routeEnd - routeStart).count();

    auto serializeStart = clock::now();
    std::string responseStr = res.toString();
    auto serializeEnd = clock::now();

    auto serializeTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(serializeEnd - serializeStart).count();

    //Logger::getInstance().log(
    //    LogLevel::INFO_LOG,
    //    "Response sent to client (" + std::to_string(responseStr.size()) + " bytes)"
    //);

    auto sendStart = clock::now();
    send(clientSocket, responseStr.c_str(), static_cast<int>(responseStr.size()), 0);
    auto sendEnd = clock::now();

    auto sendTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(sendEnd - sendStart).count();

    auto finishedAt = clock::now();

    auto processingTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(finishedAt - workerStart).count();
    auto totalTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(finishedAt - acceptedAt).count();

    m_totalRequests.fetch_add(1, std::memory_order_relaxed);
    m_totalQueueTimeMs.fetch_add(queueTimeMs, std::memory_order_relaxed);
    m_totalRecvTimeMs.fetch_add(recvTimeMs, std::memory_order_relaxed);
    m_totalParseTimeMs.fetch_add(parseTimeMs, std::memory_order_relaxed);
    m_totalRouteTimeMs.fetch_add(routeTimeMs, std::memory_order_relaxed);
    m_totalSerializeTimeMs.fetch_add(serializeTimeMs, std::memory_order_relaxed);
    m_totalSendTimeMs.fetch_add(sendTimeMs, std::memory_order_relaxed);
    m_totalProcessingTimeMs.fetch_add(processingTimeMs, std::memory_order_relaxed);
    m_totalTotalTimeMs.fetch_add(totalTimeMs, std::memory_order_relaxed);

    closesocket(clientSocket);
}
