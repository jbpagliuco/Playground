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