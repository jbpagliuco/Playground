#pragma once

#include <string>
#include <vector>

namespace refl
{
	// Function pointer type. This must include the maximum number of
	// allowed parameters (any of which can be null depending on the real reflected function definition).
	typedef void (*FunctionType)(void* functionInvoker, void* obj, void* returnValue, void* param1);

	// Registers a function with the reflection at runtime.
	class FunctionRegistration
	{
	public:
		// Registers a function.
		FunctionRegistration(const char* functionName, FunctionType function, void* functionInvoker) :
			mFunctionName(functionName),
			mFunction(function),
			mFunctionInvoker(functionInvoker)
		{}

	public:
		// Function name.
		const char* mFunctionName;

		// Pointer to the function invoker
		FunctionType mFunction;
		void* mFunctionInvoker;
	};
}