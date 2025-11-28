#include "Response.h"

std::string Response::toString() const
{
    std::string statusText;
    switch (m_statusCode)
    {
    case 200: statusText = "OK"; break;
    case 400: statusText = "Bad Request"; break;
    case 404: statusText = "Not Found"; break;
    case 500: statusText = "Internal Server Error"; break;
    default:  statusText = "Unknown"; break;
    }

    std::string res;
    res += "HTTP/1.1 " + std::to_string(m_statusCode) + " " + statusText + "\r\n";
    res += "Content-Length: " + std::to_string(m_body.size()) + "\r\n";
    res += "Content-Type: text/plain\r\n";
    res += "\r\n"; 
    res += m_body;

    return res;
}
