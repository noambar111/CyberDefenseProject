#pragma once
#include <string>
#include <map>

class Response {
    friend class ResponseBuilder;
private:
    std::string m_version;
    int m_statusCode;
    std::string m_statusText;
    std::map<std::string, std::string> m_headers;
    std::string m_body;

public:
    std::string toString() const;
};
