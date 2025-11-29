#pragma once
#include <string>
#include <map>

class Request
{
public:
    Request(const std::string& raw);

    const std::string& method()  const { return m_method; }
    const std::string& path()    const { return m_path; }
    const std::string& version() const { return m_version; }

    const std::map<std::string, std::string>& params() const { return m_params; }

private:
    void parseRequestLine(const std::string& line);
    void parseQueryString();

private:
    std::string m_raw;

    std::string m_method;
    std::string m_path;
    std::string m_version;

    std::map<std::string, std::string> m_params;
};