#pragma once

#include "Util/Serialize.h"

namespace playground
{
	// Deserialize a parameter map into an object, according to its reflected data.
	void ReflectionDeserialize(const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters);
}