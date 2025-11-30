#include "ResponseBuilder.h"

ResponseBuilder::ResponseBuilder()
{
    
}

ResponseBuilder& ResponseBuilder::setVersion(const std::string& version)
{
    // TODO: insert return statement here
}

ResponseBuilder& ResponseBuilder::setStatus(int statusCode)
{
    this->m_statusCode = statusCode;
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
}
