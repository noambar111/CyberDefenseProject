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
    case LogLevel::INFO_LOG: lvl = "INFO"; break;
    case LogLevel::WARNING_LOG: lvl = "WARNING"; break;
    case LogLevel::ERROR_LOG: lvl = "ERROR"; break;
    case LogLevel::DEBUG_LOG: lvl = "DEBUG"; break;
    }

    std::string line = ts + " [" + lvl + "] " + message;

    if (!m_file.is_open())
    {
        m_file.open("server.log", std::ios::app);
    }

    std::lock_guard<std::mutex> lock(m_lock);
    m_file << line << std::endl;
}

std::string Logger::getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm local{};
    localtime_s(&local, &t);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
        ) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d %H:%M:%S"); 
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count(); 

    return oss.str();
}
