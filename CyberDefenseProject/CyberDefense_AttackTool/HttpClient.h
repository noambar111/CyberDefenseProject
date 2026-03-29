#pragma once
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

class HttpClient {
public:
    HttpClient(const std::string& host, unsigned short port) 
        : m_host(host),
        m_port(port),
        m_socket(INVALID_SOCKET),
        m_initialized(false)
    {}
    ~HttpClient();

    bool connectToServer();
    bool sendGet(const std::string& path);
    std::string receiveResponse();
    void close();

private:
    std::string m_host;
    unsigned short m_port;
    SOCKET m_socket{ INVALID_SOCKET };
    bool m_initialized{ false };
};