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
            Logger::getInstance().log(LogLevel::INFO_LOG, "Client connected");
            handleClient(client);
            closesocket(client);
        }
    }

    closesocket(m_serverSocket);
    WSACleanup();
}

void Server::handleClient(int clientSocket)
{
    char buf[LEN];
    int bytesReceived = recv(clientSocket, buf, LEN, 0);
    Request req(std::string(buf, bytesReceived));

    Response res = m_router.route(req);

    Logger::getInstance().log(LogLevel::INFO_LOG,
        "Response sent to client (" + std::to_string(res.toString().size()) + " bytes)");
    send(clientSocket, res.toString().c_str(), res.toString().size(), 0);
}
