#include "FileLogger.h"
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

FileLogger& FileLogger::Get() 
{
    static FileLogger instance;
    if (!instance.myIsInitialized)
    {
        instance.Initialize();
    }
    return instance;
}

void FileLogger::Initialize()
{
    std::string baseName = "../Log";
    std::string extension = ".txt";
    std::string fileName = baseName + extension;

    int counter = 0;
    std::ifstream testFile(fileName);
    while (testFile.is_open())
    {
        testFile.close();
        std::ostringstream oss;
        oss << baseName << counter << extension;
        fileName = oss.str();
        counter++;
        testFile.open(fileName);
    }

    myFile = fileName;

    myLog.open(fileName);
    std::string date = GetDateStamp();
    myLog << GetDateStamp() << ": Linnea Engine Log\n" << std::endl;
    Flush();

    myIsInitialized = true;
}

FileLogger::~FileLogger() 
{
    if (myLog.is_open()) 
    {
        myLog << Timestamp() << " INFO: Logger destructed" << std::endl;
        myLog.close();
    }
}

std::string FileLogger::Timestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime, &t);
    char buf[16];

    // Format only time: HH:MM:SS
    strftime(buf, sizeof(buf), "%H:%M:%S", &localTime);

    return std::string(buf);
}

std::string FileLogger::GetDateStamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime, &t);

    char buf[16];
    // Format only date: YYYY-MM-DD
    strftime(buf, sizeof(buf), "%Y-%m-%d", &localTime);

    return std::string(buf);
}

void FileLogger::Log(const std::string& message) 
{
    if (!myLog.is_open())
    {
        myLog.open(myFile, std::ios::app);
    }
    myLog << Timestamp() << " INFO: " << message << std::endl;
}

void FileLogger::LogWarning(const std::string& message) 
{
    if (!myLog.is_open())
    {
        myLog.open(myFile, std::ios::app);
    }
    myLog << Timestamp() << " WARNIG: " << message << std::endl;
}

void FileLogger::LogError(const std::string& message) 
{
    if (!myLog.is_open())
    {
        myLog.open(myFile, std::ios::app);
    }
    myLog << Timestamp() << " ERROR: " << message << std::endl;
}

void FileLogger::Flush() 
{
    if (myLog.is_open()) 
    {
        myLog.flush();
    }
}
