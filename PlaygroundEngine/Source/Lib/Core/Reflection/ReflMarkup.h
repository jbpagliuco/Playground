#pragma once

// CppRefl includes
#include "ReflectionMarkup.h"

// Sets the display name of a reflected element.
#define REFL_ATTR_NAME "name"
#define REFL_NAME(displayName) REFL_ATTR(REFL_ATTR_NAME, displayName)