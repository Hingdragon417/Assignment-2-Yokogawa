#pragma once
#include <string>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    Logger() = default; 
    Logger(const std::string& logPath); 

    void Log(const std::string& message);
    void Log(const std::string& message, LogLevel level);

private:
    std::string logPath;

    std::string MakeFinalLogPath(const std::string& logPath) const;
	void TrimOldestLinesForAppend(int maxMessages) const;
    std::size_t countLines(const std::string& path) const;
};

