
// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

#include "ReflectionRegistry.h"

#include "EngineReflection.reflgen.h"

#include "EngineReflection.cpp"

void EngineReflection_InitReflection(refl::Registry* registry)
{
	if (registry == nullptr) {
		registry = refl::GetSystemRegistry();
	}

	// Registers every reflected class in this project.
	playground::CameraComponent::__ReflRegisterClass(registry);

	// Registers every reflected global function in this project.
	

	registry->Finalize();
}

#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
