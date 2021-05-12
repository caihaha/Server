#ifndef _CELL_LOG_H_
#define _CELL_LOG_H_

#include "CELLTask.h"
#include <ctime>
#include <chrono>

class CellLog
{
	//Info  普通信息
	//Debug 调试信息，只在debug模式起作用
	//Warring 警告信息
	//Error 错误信息
#ifdef _DEBUG
#ifndef CellLog_Debug
#define CellLog_Debug(...) CellLog::Debug(__VA_ARGS__)
#endif
#else
#ifndef CellLog_Debug
#define CellLog_Debug(...)
#endif
#endif // _DEBUG

#define CellLog_Info(...) CellLog::Info(__VA_ARGS__)
#define CellLog_Warring(...) CellLog::Warring(__VA_ARGS__)
#define CellLog_Error(...) CellLog::Error(__VA_ARGS__)
#define CellLog_Error(...) CellLog::Error(__VA_ARGS__)
#define CellLog_PError(...) CellLog::PError(__VA_ARGS__)

private:
	CellLog()
	{
		_taskServer.Start();
	}

	~CellLog()
	{
		_taskServer.Close();
		if (_logFile)
		{
			Info("CellLog fclose(_logFile)");
			fclose(_logFile);
			_logFile = nullptr;
		}
	}
public:
	static CellLog& Instance()
	{
		static  CellLog sLog;
		return sLog;
	}

	void setLogPath(const char* logName, const char* mode, bool hasDate)
	{
		if (_logFile)
		{
			Info("CellLog::setLogPath _logFile != nullptr");
			fclose(_logFile);
			_logFile = nullptr;
		}
		//
		static char logPath[256] = {};
		//
		if (hasDate)
		{
			auto t = std::chrono::system_clock::now();
			auto tNow = std::chrono::system_clock::to_time_t(t);
			std::tm* now = std::localtime(&tNow);
			sprintf(logPath, "%s[%d-%d-%d_%d-%d-%d].txt", logName, now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
		}
		else {
			sprintf(logPath, "%s.txt", logName);
		}

		//
		_logFile = fopen(logPath, mode);
		if (_logFile)
		{
			Info("CellLog::setLogPath success,<%s,%s>", logPath, mode);
		}
		else {
			Info("CellLog::setLogPath failed,<%s,%s>", logPath, mode);
		}
	}

	static void PError(const char* pStr)
	{
		PError("%s", pStr);
	}

	template<typename ...Args>
	static void PError(const char* pformat, Args ... args)
	{
#ifdef _WIN32
		auto errCode = GetLastError();

		Instance()._taskServer.addTask([=]() {

			char text[256] = {};
			FormatMessageA(
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				errCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&text, 256, NULL
			);

			EchoReal(true, "PError ", pformat, args...);
			EchoReal(false, "PError ", "errno=%d,errmsg=%s", errCode, text);
			});
#else
		auto errCode = errno;
		Instance()._taskServer.addTask([=]() {
			EchoReal(true, "PError ", pformat, args...);
			EchoReal(true, "PError ", "errno<%d>,errmsg<%s>", errCode, strerror(errCode));
			});
#endif
	}

	static void Error(const char* pStr)
	{
		Error("%s", pStr);
	}

	template<typename ...Args>
	static void Error(const char* pformat, Args ... args)
	{
		Echo("Error ", pformat, args...);
	}

	static void Warring(const char* pStr)
	{
		Warring("%s", pStr);
	}

	template<typename ...Args>
	static void Warring(const char* pformat, Args ... args)
	{
		Echo("Warring ", pformat, args...);
	}

	static void Debug(const char* pStr)
	{
		Debug("%s", pStr);
	}

	template<typename ...Args>
	static void Debug(const char* pformat, Args ... args)
	{
		Echo("Debug ", pformat, args...);
	}

	static void Info(const char* pStr)
	{
		Info("%s", pStr);
	}

	template<typename ...Args>
	static void Info(const char* pformat, Args ... args)
	{
		Echo("Info ", pformat, args...);
	}

	template<typename ...Args>
	static void Echo(const char* type, const char* pformat, Args ... args)
	{
		CellLog* pLog = &Instance();
		pLog->_taskServer.addTask([=]() {
			EchoReal(true, type, pformat, args...);
			});
	}

	template<typename ...Args>
	static void EchoReal(bool br, const char* type, const char* pformat, Args ... args)
	{
		CellLog* pLog = &Instance();
		if (pLog->_logFile)
		{
			auto t = system_clock::now();
			auto tNow = system_clock::to_time_t(t);
			//fprintf(pLog->_logFile, "%s", ctime(&tNow));
			std::tm* now = std::localtime(&tNow);
			if (type)
				fprintf(pLog->_logFile, "%s", type);
			fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
			fprintf(pLog->_logFile, pformat, args...);
			if (br)
				fprintf(pLog->_logFile, "%s", "\n");
			fflush(pLog->_logFile);

		}
		if (type)
			printf("%s", type);
		printf(pformat, args...);
		if (br)
			printf("%s", "\n");
	}
private:
	FILE* _logFile = nullptr;
	CellTaskServer _taskServer;
};

#endif // !_CELL_LOG_H_
