#include "HttpClient.h"
#include <iostream>

HttpClient::~HttpClient()
{
	close();
}

bool HttpClient::connectToServer()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cerr << "WSAStartup failed\n";
		return false;
	}
	m_initialized = true;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cerr << "socket() failed\n";
		return false;
	}

	sockaddr_in server{};
	server.sin_family = AF_INET;
	server.sin_port = htons(m_port);
	if (inet_pton(AF_INET, m_host.c_str(), &server.sin_addr) <= 0)
	{
		std::cerr << "Invalid address\n";
		return false;
	}
	if (connect(m_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cerr << "connect() failed\n";
	}
	return true;
}

bool HttpClient::sendGet(const std::string& path)
{
	std::string request =
		"GET " + path + " HTTP/1.1\r\n"
		"Host: " + m_host + "\r\n"
		"Connection: Close\r\n\r\n";
	int result = send(m_socket,
		request.c_str(),
		static_cast<int>(request.size()),
		0);

	if (result == SOCKET_ERROR)
	{
		std::cerr << "send() failed\n";
		return false;
	}

	return true;
}

void HttpClient::close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	if (m_initialized)
	{
		WSACleanup();
		m_initialized = false;
	}
}
