#pragma once
#include <string>
#include <map>
#include "Response.h"

class ResponseBuilder
{
public:
    ResponseBuilder();
    ResponseBuilder& setVersion(const std::string& version);
    ResponseBuilder& setStatus(int statusCode);
    ResponseBuilder& setBody(const std::string& body);
    ResponseBuilder& addHeader(const std::string& key, const std::string& value);

    Response build() const;

private:
    std::string m_version;
    int m_statusCode = 200;
    std::string m_statusText;
    std::string m_body;
    std::map<std::string, std::string> m_headers;
};