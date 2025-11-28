#pragma once
#include <winsock2.h>

class Server {
public:
    Server(int port) : m_port(port) {};
    void start();

private:
    int m_port;
    void handleClient(int clientSocket);
    SOCKET m_serverSocket;

};
