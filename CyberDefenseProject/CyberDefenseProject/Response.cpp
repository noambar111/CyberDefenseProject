#include "Response.h"

std::string Response::toString() const
{
    std::string res;

    res += "HTTP/" + m_version + " "
        + std::to_string(m_statusCode) + " "
        + m_statusText + "\r\n";

    for (const auto& [key, value] : m_headers)
    {
        res += key + ": " + value + "\r\n";
    }

    res += "\r\n";

    res += m_body;

    return res;
}
