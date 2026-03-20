#pragma once
#include <winsock2.h>
#include "Router.h"
#include "Logger.h"
#include "ThreadPool.h"
#define NUM_OF_THREADS 5

class Server {
public:
    Server(int port) : m_port(port), m_pool(100) {};
    void start();

private:
    int m_port;
    void handleClient(int clientSocket, std::chrono::steady_clock::time_point acceptedAt);    
    SOCKET m_serverSocket;
    Router m_router;
    ThreadPool m_pool;
    std::atomic<size_t> m_totalRequests{ 0 };
    std::atomic<long long> m_totalQueueTimeMs{ 0 };
    std::atomic<long long> m_totalRouteTimeMs{ 0 };
    std::atomic<long long> m_totalProcessingTimeMs{ 0 };
    std::atomic<long long> m_totalTotalTimeMs{ 0 };
    std::atomic<long long> m_totalRecvTimeMs{ 0 };
    std::atomic<long long> m_totalParseTimeMs{ 0 };
    std::atomic<long long> m_totalSerializeTimeMs{ 0 };
    std::atomic<long long> m_totalSendTimeMs{ 0 };

};
