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

    std::atomic<long long> m_totalAccepted{ 0 };
    std::atomic<long long> m_totalEnqueued{ 0 };
    std::atomic<long long> m_totalStarted{ 0 };
    std::atomic<long long> m_totalCompleted{ 0 };

    std::atomic<long long> m_activeWorkers{ 0 };

    std::atomic<long long> m_totalQueueTimeUs{ 0 };
    std::atomic<long long> m_totalRecvTimeUs{ 0 };
    std::atomic<long long> m_totalParseTimeUs{ 0 };
    std::atomic<long long> m_totalRouteTimeUs{ 0 };
    std::atomic<long long> m_totalSerializeTimeUs{ 0 };
    std::atomic<long long> m_totalSendTimeUs{ 0 };
    std::atomic<long long> m_totalProcessingTimeUs{ 0 };
    std::atomic<long long> m_totalTotalTimeUs{ 0 };

};
