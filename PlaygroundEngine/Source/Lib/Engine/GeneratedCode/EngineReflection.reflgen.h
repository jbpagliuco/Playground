
#pragma once

// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

// Forward declare.
namespace refl {
	class Registry;
}

void EngineReflection_InitReflection(refl::Registry* registry = nullptr);

#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
