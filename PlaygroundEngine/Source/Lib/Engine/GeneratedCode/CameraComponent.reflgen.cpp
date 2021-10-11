
// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

#include "ReflectionRegistry.h"

#include "Components/CameraComponent.h"


void playground::CameraComponent::__ReflRegisterClass(refl::Registry& registry)
{
	// Class data
	refl::ClassRegistration classRegistration(CameraComponent::CLASS_QUALIFIED_NAME, sizeof(CameraComponent));

	// Field data
	

	// Function data
	

	// Finalize runtime specific data
	registry.FinalizeClass(classRegistration);
}


#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
