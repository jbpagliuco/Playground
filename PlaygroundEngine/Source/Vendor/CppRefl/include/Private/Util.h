#pragma once

#include <string>
#include <vector>

#include "ReflectionTypes.h"

#define REFL_ARRAY_COUNT(Array) refl::util::CountOfArray(Array)

namespace refl
{
	class Class;
	class FileIO;
	class Registry;

	namespace util
	{
		template <typename T, size_t N>
		constexpr size_t CountOfArray(T(&)[N])
		{
			return N;
		}

		void SerializeTypeArray(class FileIO& io, std::vector<const Type*>& types, const class Registry& registry);
		void SerializeTypeInstanceArray(class FileIO& io, std::vector<TypeInstance>& types, const class Registry& registry);

		// Serialize some reflected data to a custom format. (Eventually this should use XML, JSON, etc.)
		std::string Serialize(const Registry& reflRegistry, const Class& reflClass, void* data);
	}
}