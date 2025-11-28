#pragma once

class Server {
public:
    Server(int port);
    void start();

private:
    int m_port;
    void handleClient(int clientSocket);
};
