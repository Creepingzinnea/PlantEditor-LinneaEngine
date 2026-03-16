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
	void Log(const std::string& message, unsigned long aHresult);
	void LogWarning(const std::string& message, unsigned long aHresult);
	void LogError(const std::string& message, unsigned long aHresult);

	void Flush();

private:
	enum class eWarningLevel
	{
		Info = 0,
		Warning = 1,
		Error = 2
	};

	void Log(const eWarningLevel aWarningLevel, const std::string& message);
	void LogHRESULT(const eWarningLevel aWarningLevel, const std::string& message, unsigned long aHresult);
	std::string GetDateStamp();
	std::string Timestamp();
	void Initialize();
	std::string BuildHresultMessage(unsigned long aHresult);

	FileLogger() = default;
	~FileLogger();
	FileLogger(const FileLogger&) = delete;
	FileLogger& operator=(const FileLogger&) = delete;

	std::string myFilePath = "";
	std::ofstream myLog;

	bool myIsInitialized = false;
};

