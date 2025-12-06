#pragma once
#include <winsock2.h>
#include "Router.h"
#include "Logger.h"

class Server {
public:
    Server(int port) : m_port(port) {};
    void start();

private:
    int m_port;
    void handleClient(int clientSocket);
    SOCKET m_serverSocket;
    Router m_router;

};
