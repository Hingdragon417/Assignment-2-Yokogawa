#include "Logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>

void Logger::Log(const std::string& message)
{
    Log(message, LogLevel::INFO);
}

std::size_t countLines(const std::string& path) {
    std::ifstream f(path);
    if (!f) return 0;
    return std::count(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>(), '\n');
}

void TrimOldestLinesForAppend() {
    const std::string fileName = "logfile";
    const std::string finalName = fileName + ".txt";
    const int MAX_MESSAGES = 5;

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

    if (static_cast<int>(lines.size()) < MAX_MESSAGES) {
        return;
    }

    if (!lines.empty()) {
        lines.erase(lines.begin());
    }

    std::ofstream outfile(finalName, std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "TrimOldestLinesForAppend: Error opening file for writing: " << finalName << std::endl;
        return;
    }

    for (const auto& l : lines) {
        outfile << l << '\n';
    }
    outfile.close();
}

void Logger::Log(const std::string& message, LogLevel level)
{
    TrimOldestLinesForAppend();

    std::string fileName = "logfile";
    std::string finalName = fileName + ".txt";

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

