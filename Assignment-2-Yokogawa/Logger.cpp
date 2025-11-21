#include "Logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>

Logger::Logger(const std::string& logPath) : logPath(logPath) {}

void Logger::Log(const std::string& message) {
    Log(message, LogLevel::INFO);
}

std::size_t Logger::countLines(const std::string& path) const {
    std::ifstream f(path);
    if (!f) return 0;
    return std::count(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>(), '\n');
}

void Logger::TrimOldestLinesForAppend(int maxMessages) const {
    const std::string finalName = MakeFinalLogPath(this->logPath);

    std::ifstream infile(finalName);
    if (!infile.is_open()) {
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();

    if (static_cast<int>(lines.size()) + 1 <= maxMessages) {
        return;
    }

    std::size_t removeCount = (lines.size() + 1) - static_cast<std::size_t>(maxMessages);
    if (removeCount >= lines.size()) {
        std::ofstream ofs(finalName, std::ios::trunc);
        return;
    }

    std::ofstream outfile(finalName, std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "TrimOldestLinesForAppend: Error opening file for writing: " << finalName << std::endl;
        return;
    }

    for (std::size_t i = removeCount; i < lines.size(); ++i) {
        outfile << lines[i] << '\n';
    }
    outfile.close();
}

std::string Logger::MakeFinalLogPath(const std::string& logPath) const{
    if (logPath.empty()) {
        return "logfile.txt";
    }
    char last = logPath.back();
    if (last == '/' || last == '\\') {
        return logPath + "logfile.txt";
    }
    if (logPath.size() >= 4 && logPath.substr(logPath.size() - 4) == ".txt") {
        return logPath;
    }
    return logPath + ".txt";
}

void Logger::Log(const std::string& message, LogLevel level) {
    const int MAX_MESSAGES = 5;

    TrimOldestLinesForAppend(MAX_MESSAGES);

    std::string finalName = MakeFinalLogPath(logPath);

    std::ofstream outfile(finalName, std::ios::app);
    if (!outfile) {
        std::cerr << "Error opening log file for writing: " << finalName << std::endl;
        return;
    }

    std::string levelStr;
    switch (level) {
        case LogLevel::INFO: levelStr = "[INFO] "; break;
        case LogLevel::WARNING: levelStr = "[WARNING] "; break;
        case LogLevel::ERROR: levelStr = "[ERROR] "; break;
        default: levelStr = "[UNKNOWN] "; break;
    }

    outfile << levelStr << message << std::endl;
}