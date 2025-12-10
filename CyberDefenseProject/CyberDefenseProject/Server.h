#pragma once
#include <winsock2.h>
#include "Router.h"
#include "Logger.h"
#include "ThreadPool.h"
#define NUM_OF_THREADS 5

class Server {
public:
    Server(int port) : m_port(port), m_pool(NUM_OF_THREADS) {};
    void start();

private:
    int m_port;
    void handleClient(int clientSocket);
    SOCKET m_serverSocket;
    Router m_router;
    ThreadPool m_pool;

};
