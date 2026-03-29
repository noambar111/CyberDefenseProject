#pragma once
#include <mutex>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

enum class LogLevel {
	INFO_LOG,
	WARNING_LOG,
	ERROR_LOG,
	DEBUG_LOG
};

class Logger 
{
public:
	static Logger& getInstance();
	void log(LogLevel level, const std::string& message);
private:
	Logger() {}
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	std::string getTimestamp();

	std::ofstream m_file;
	std::mutex m_lock;
};