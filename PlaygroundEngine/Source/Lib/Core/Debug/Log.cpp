#include "Log.h"

#include <stdio.h>
#include <string>

#include "OS/OSWin32.h"
#include "Util/Util.h"

namespace playground
{
	static void LogToOutputWindow(const std::string &filter, const std::string &message)
	{
		size_t garbage;

		wchar_t wFilter[256];
		mbstowcs_s(&garbage, wFilter, filter.c_str(), STATIC_ARRAY_SIZE(wFilter));
		OutputDebugString(wFilter);

		wchar_t wMessage[4096];
		mbstowcs_s(&garbage, wMessage, message.c_str(), STATIC_ARRAY_SIZE(wMessage));
		OutputDebugString(wMessage);

		OutputDebugString(L"\r\n");
	}

	void LogInfoV(const std::string &filter, const char *format, va_list args)
	{
		const std::string filterFormatted = "[" + filter + "]";

		char message[4096];
		vsprintf_s(message, format, args);

		printf("%s%s\r\n", filterFormatted.c_str(), message);

		LogToOutputWindow(filterFormatted, message);
	}

	void LogInfo(const std::string &filter, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		LogInfoV(filter, format, args);
		va_end(args);
	}

	void LogErrorV(const std::string &filter, const char *format, va_list args)
	{
		const std::string filterFormatted = "[" + filter + "]";

		char message[4096];
		vsprintf_s(message, format, args);

		// Print with red color
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		WORD saved_attributes;

		// Save current attributes
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		printf("%s%s\r\n", filterFormatted.c_str(), message);

		// Restore original attributes
		SetConsoleTextAttribute(hConsole, saved_attributes);

		LogToOutputWindow(filterFormatted, message);
	}

	void LogError(const std::string &filter, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		LogErrorV(filter, format, args);
		va_end(args);
	}

	void LogLineBreak()
	{
		printf("\r\n");
		OutputDebugString(L"\r\n");
	}
}