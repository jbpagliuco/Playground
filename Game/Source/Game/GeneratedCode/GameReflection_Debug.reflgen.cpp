
// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

#include "ReflectionRegistry.h"

#include "GameReflection_Debug.reflgen.h"

#include "GameReflection.cpp"

void GameReflection_Debug_InitReflection(refl::Registry& registry)
{
	// Registers every reflected class in this project.
	

	// Registers every reflected global function in this project.
	

	registry.Finalize();
}

#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
