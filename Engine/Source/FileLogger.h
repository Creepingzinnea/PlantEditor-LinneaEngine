#pragma once
#include <fstream>
#include "string.h"

class FileLogger
{
public:
    static FileLogger& Get();
    void Log(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
    void Flush();
    std::string GetDateStamp();

private:
    std::string Timestamp();
    void Initialize();

    FileLogger() = default;
    ~FileLogger();
    FileLogger(const FileLogger&) = delete;           
    FileLogger& operator=(const FileLogger&) = delete; 

    std::string myFile = "../log.txt";
    std::ofstream myLog;

    bool myIsInitialized = false;
};

