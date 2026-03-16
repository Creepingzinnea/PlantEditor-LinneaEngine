#include "FileLogger.h"
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <Windows.h>

//#define DO_NOT_OVERWRITE_LOG

FileLogger& FileLogger::Get()
{
	static FileLogger instance;
	if (!instance.myIsInitialized)
	{
		instance.Initialize();
	}
	return instance;
}

void FileLogger::Log(const std::string& message)
{
	Log(eWarningLevel::Info, message);
}

void FileLogger::LogWarning(const std::string& message)
{
	Log(eWarningLevel::Warning, message);
}

void FileLogger::LogError(const std::string& message)
{
	Log(eWarningLevel::Error, message);
}

void FileLogger::Log(const std::string& message, unsigned long aHresult)
{
	LogHRESULT(eWarningLevel::Info, message, aHresult);
}

void FileLogger::LogWarning(const std::string& message, unsigned long aHresult)
{
	LogHRESULT(eWarningLevel::Warning, message, aHresult);
}

void FileLogger::LogError(const std::string& message, unsigned long aHresult)
{
	LogHRESULT(eWarningLevel::Error, message, aHresult);
}

void FileLogger::Initialize()
{
	std::string baseName = "../Log";
	std::string extension = ".txt";
	std::string fileName = baseName + extension;

#ifdef DO_NOT_OVERWRITE_LOG
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
#endif

	myFilePath = fileName;

	myLog.open(fileName);
	myLog << GetDateStamp() << ": Linnea Engine Log :) \n";
	Flush();

	myIsInitialized = true;
}

std::string FileLogger::BuildHresultMessage(unsigned long aHresult)
{
	std::ostringstream stream;
	stream << "HRESULT: 0x" << std::hex << aHresult;

	char* errorMsg = nullptr;

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, aHresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorMsg, 0, NULL);

	if (errorMsg)
	{
		stream << " Message: " << errorMsg;
		LocalFree(errorMsg);
	}

	return stream.str();
}

FileLogger::~FileLogger()
{
	if (myLog.is_open())
	{
		myLog << Timestamp() << " INFO: Logger destructed" << std::endl; //std::endl will force flush
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

void FileLogger::Log(const eWarningLevel aWarningLevel, const std::string& message)
{
	if (!myLog.is_open())
	{
		myLog.open(myFilePath, std::ios::app);
	}

	const char* level = "";

	switch (aWarningLevel)
	{
	case eWarningLevel::Info: level = "INFO: "; break;
	case eWarningLevel::Warning: level = "WARNING: "; break;
	case eWarningLevel::Error: level = "ERROR: "; break;
	}

	myLog << Timestamp() << " " << level << message << "\n";
}

void FileLogger::LogHRESULT(const eWarningLevel aWarningLevel, const std::string& message, unsigned long aHresult)
{
	const std::string hresult = BuildHresultMessage(aHresult);
	Log(aWarningLevel, message + ". " + hresult);
}

void FileLogger::Flush()
{
	if (myLog.is_open())
	{
		myLog.flush();
	}
}
