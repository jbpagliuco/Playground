#pragma once

#include <string>

#define REFL_ARRAY_COUNT(Array) refl::util::CountOfArray(Array)

namespace refl
{
	class Class;
	class Registry;

	namespace util
	{
		template <typename T, size_t N>
		constexpr size_t CountOfArray(T(&)[N])
		{
			return N;
		}

		// Serialize some reflected data to a custom format. (Eventually this should use XML, JSON, etc.)
		std::string Serialize(const Registry& reflRegistry, const Class& reflClass, void* data);
	}
}