#pragma once

// Raise an error. Used by internal code.
#define REFL_INTERNAL_RAISE_WARNING(fmt, ...) refl::RaiseErrorInternal(refl::ErrorType::WARNING, __FILE__, __LINE__, fmt, __VA_ARGS__)
#define REFL_INTERNAL_RAISE_ERROR(fmt, ...)   refl::RaiseErrorInternal(refl::ErrorType::ERROR,   __FILE__, __LINE__, fmt, __VA_ARGS__)

namespace refl
{
	enum class ErrorType
	{
		WARNING = 0,
		ERROR
	};

	void RaiseErrorInternal(ErrorType errorType, const char* file, int line, const char* fmt, ...);

	// Sets a custom error handler.
	typedef void(*ErrorHandler)(ErrorType errorType, const char* message, const char* file, int line);
	void SetErrorHandler(ErrorHandler errorHandler);
}