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
    void Log(const std::string& message);
    void Log(const std::string& message, LogLevel level);
};

