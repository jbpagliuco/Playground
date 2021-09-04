#include "Assert.h"

#include <cstdlib>
#include <stdio.h>

#include "Log.h"
#include "OS/OSWin32.h"

#define ASSERT_LOG_FILTER "Asserts"

#define LOG_ASSERT_MESSAGE(fmt, ...)							\
	playground::LogError(ASSERT_LOG_FILTER, fmt, __VA_ARGS__);			\
	playground::LogError(ASSERT_LOG_FILTER, "Filename: %s", file);		\
	playground::LogError(ASSERT_LOG_FILTER, "Line: %d", line)		


namespace playground
{
	bool PrintAssertMessage(const char* assert, const char* file, int line)
	{
		LOG_ASSERT_MESSAGE("%s", assert);

		ShowCursor(true);

		switch (MessageBoxA(nullptr, assert, "Assert Failed", MB_YESNOCANCEL)) {
		case IDYES:
			return true;

		case IDNO:
			return false;

		case IDCANCEL:
			exit(-1);
			return false;
		}

		return true;
	}

	bool PrintAssertMessage(const char* assert, const char* file, int line, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		char message[4096];
		vsprintf_s(message, format, args);
		va_end(args);

		LOG_ASSERT_MESSAGE("%s, %s", assert, message);

		ShowCursor(true);

		switch (MessageBoxA(nullptr, message, "Assert Failed", MB_YESNOCANCEL)) {
		case IDYES:
			return true;

		case IDNO:
			return false;

		case IDCANCEL:
			exit(-1);
			return false;
		}

		return true;
	}

	void PrintErrorMessage(const char* assert, const char* file, int line)
	{
		LOG_ASSERT_MESSAGE("%s", assert);

		MessageBoxA(nullptr, assert, "Fatal Error", MB_OK);
	}

	void PrintErrorMessage(const char* assert, const char* file, int line, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		char message[4096];
		vsprintf_s(message, format, args);
		va_end(args);

		LOG_ASSERT_MESSAGE("%s, %s", assert, message);

		MessageBoxA(nullptr, message, "Fatal Error", MB_OK);
	}

	bool IsDebuggerAttached()
	{
#if CORE_PLATFORM(PC)
		return IsDebuggerPresent();
#endif

		return false;
	}
}