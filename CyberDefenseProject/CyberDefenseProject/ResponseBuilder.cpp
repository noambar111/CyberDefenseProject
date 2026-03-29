#include "ResponseBuilder.h"

ResponseBuilder::ResponseBuilder()
{
    m_version = "HTTP/1.1";
    m_statusCode = 200;
    m_statusText = "OK";
}

ResponseBuilder& ResponseBuilder::setVersion(const std::string& version)
{
    this->m_version = version;
    return *this;
}

ResponseBuilder& ResponseBuilder::setStatus(int statusCode)
{
    this->m_statusCode = statusCode;
    switch (statusCode)
    {
        case 200: m_statusText = "OK"; break;
        case 400: m_statusText = "Bad Request"; break;
        case 404: m_statusText = "Not Found"; break;
        case 500: m_statusText = "Internal Server Error"; break;
        default: m_statusText = "Unknown";  break;
    }
    return *this;
}

ResponseBuilder& ResponseBuilder::setBody(const std::string& body)
{
    this->m_body = body;
    return *this;
}

ResponseBuilder& ResponseBuilder::addHeader(const std::string& key, const std::string& value)
{
    this->m_headers[key] = value;
    return *this;
}

Response ResponseBuilder::build() const
{
    Response res;
    res.m_body = this->m_body;
    res.m_headers = this->m_headers;
    res.m_statusCode = this->m_statusCode;
    res.m_statusText = this->m_statusText;
    res.m_version = this->m_version;
    return res;
}
