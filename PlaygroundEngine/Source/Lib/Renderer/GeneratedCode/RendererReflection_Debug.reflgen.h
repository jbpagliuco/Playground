
#pragma once

// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

#include "ReflectionRegistry.h"

void RendererReflection_Debug_InitReflection(refl::Registry& registry = refl::GetSystemRegistry());

#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
