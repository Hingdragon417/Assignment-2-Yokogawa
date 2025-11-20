#include "Logger.h"
#include <string>
#include <iostream>

void Logger::Log(const std::string& message)
{
	std::cerr << "Logger: " << message << std::endl;
}
