#pragma once
#include <string>
#include <map>
#include "Response.h"

class ResponseBuilder
{
public:
    ResponseBuilder& setStatus(int statusCode);
    ResponseBuilder& setBody(const std::string& body);
    ResponseBuilder& addHeader(const std::string& key, const std::string& value);

    Response build() const;

private:
    int m_statusCode = 200;
    std::string m_body;
    std::map<std::string, std::string> m_headers;
};