#pragma once

#include <stdlib.h>

#include "Core/Core.h"

// Always defined, doesn't cost anything at runtime.
#define CORE_COMPILE_TIME_ASSERT(cond, msg) static_assert(cond, msg)

#if CORE_CONFIG(DEBUG)

#define DEBUG_BREAK __debugbreak

namespace playground
{
	bool PrintAssertMessage(const char* assert, const char* file, int line);
	bool PrintAssertMessage(const char* assert, const char* file, int line, const char* format, ...);

	void PrintErrorMessage(const char* assert, const char* file, int line);
	void PrintErrorMessage(const char* assert, const char* file, int ilne, const char* format, ...);

	bool IsDebuggerAttached();
}

#define CORE_PRINT_ASSERT_MESSAGE(cond, ...) playground::PrintAssertMessage(#cond, __FILE__, __LINE__, __VA_ARGS__)

#define CORE_ASSERT(cond, ...)											\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
	}																	

#define CORE_ASSERT_RETURN(cond, ...)        	    						\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
		return; 														\
	}																	

#define CORE_ASSERT_RETURN_VALUE(cond, rv, ...)							\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
		return rv;														\
	}																	

#define CORE_ASSERT_CONTINUE(cond, ...)									\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
		continue;														\
	}																	

#define CORE_FATAL_ERROR(cond, ...)												\
	if (!(cond)) {																\
		playground::PrintErrorMessage(#cond, __FILE__, __LINE__, __VA_ARGS__);	\
		if (playground::IsDebuggerAttached()) { DEBUG_BREAK(); }					\
		exit(EXIT_FAILURE);														\
	}																	



#define CORE_ASSERT_RETURN_CLEANUP(cond, shutdownFunc, ...)				\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
		shutdownFunc();													\
		return; 														\
	}																	

#define CORE_ASSERT_RETURN_VALUE_CLEANUP(cond, rv, shutdownFunc, ...)		\
	if (!(cond)) {														\
		if (CORE_PRINT_ASSERT_MESSAGE(cond, __VA_ARGS__)) {				\
			DEBUG_BREAK();												\
		}																\
		shutdownFunc();													\
		return rv; 														\
	}																	


#else

#define CORE_ASSERT(...)
#define CORE_ASSERT_RETURN(cond, ...)					if (!(cond)) { return; }
#define CORE_ASSERT_RETURN_VALUE(cond, rv, ...)		if (!(cond)) { return rv; }
#define CORE_ASSERT_CONTINUE(cond, ...)				if (!(cond)) { continue; }
#define CORE_FATAL_ERROR(cond, ...)					if (!(cond)) { exit(EXIT_FAILURE); }

#define CORE_ASSERT_RETURN_CLEANUP(cond, shutdownFunc, ...)					if (!(cond)) { shutdownFunc(); return; }
#define CORE_ASSERT_RETURN_VALUE_CLEANUP(cond, rv, shutdownFunc, ...)			if (!(cond)) { shutdownFunc(); return rv; }

#endif // CORE_CONFIG(DEBUG)