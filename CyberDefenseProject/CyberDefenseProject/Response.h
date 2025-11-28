#pragma once
#include <string>

class Response {
public:
    Response(int code, const std::string& body) : m_statusCode(code), m_body(body) {};

    std::string toString() const;

private:
    int m_statusCode;
    std::string m_body;
};