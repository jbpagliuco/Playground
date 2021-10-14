#pragma once

// CppRefl includes
#include "ReflectionMarkup.h"

// Sets the display name of a reflected element.
#define REFL_ATTR_NAME "name"
#define REFL_NAME(displayName) REFL_ATTR(REFL_ATTR_NAME, displayName)

// Multiplies a reflected value after serialization.
#define REFL_ATTR_MULTIPLIER "multiplier"
#define REFL_MULTIPLIER(multiplier) REFL_ATTR(REFL_ATTR_MULTIPLIER, #multiplier)

// Converts a float value in degrees to radians.
#define REFL_ATTR_RADIANS REFL_MULTIPLIER(0.0174533f)

#define REFL_ATTR_ENUM_MATCH_NAME "enum_match_name"
#define REFL_ATTR_ENUM_MATCH_VAR "enum_match_var"
#define REFL_ATTR_ENUM_MATCH_MATCHES "enum_match_matches"
#define REFL_ENUM_MATCH(name, enumVar, ...) REFL_ATTR(REFL_ATTR_ENUM_MATCH_NAME, name) REFL_ATTR(REFL_ATTR_ENUM_MATCH_VAR, #enumVar) REFL_ATTR(REFL_ATTR_ENUM_MATCH_MATCHES, #__VA_ARGS__) 