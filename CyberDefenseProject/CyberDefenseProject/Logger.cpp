#include "Logger.h"

Logger& Logger::getInstance()
{
    static Logger ins;
    return ins;
}

void Logger::log(LogLevel level, const std::string& message)
{
    std::string ts = getTimestamp();
    std::string lvl;

    switch (level)
    {
    case LogLevel::INFO: lvl = "INFO"; break;
    case LogLevel::WARNING: lvl = "WARNING"; break;
    case LogLevel::ERROR: lvl = "ERROR"; break;
    case LogLevel::DEBUG: lvl = "DEBUG"; break;
    }

    std::string line = ts + " [" + lvl + "] " + message;

    if (!m_file.is_open())
    {
        m_file.open("server.log", std::ios::app);
    }

    m_file << line << std::endl;
}

std::string Logger::getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm local = *std::localtime(&t);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
        ) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d %H:%M:%S"); 
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count(); 

    return oss.str();
}
