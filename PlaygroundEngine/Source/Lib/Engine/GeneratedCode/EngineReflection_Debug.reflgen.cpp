
// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

#if !defined(REFL_BUILD_REFLECTION)

#include "ReflectionRegistry.h"

#include "EngineReflection_Debug.reflgen.h"

#include "EngineReflection.cpp"

void EngineReflection_Debug_InitReflection(refl::Registry& registry)
{
	// Registers every reflected class in this project.
	playground::BasicMovementComponent::__ReflRegisterClass(registry);
	playground::CameraComponent::__ReflRegisterClass(registry);
	playground::DirectionalLightComponent::__ReflRegisterClass(registry);
	playground::PointLightComponent::__ReflRegisterClass(registry);
	playground::RigidbodyComponent::__ReflRegisterClass(registry);
	playground::SkyboxComponent::__ReflRegisterClass(registry);
	playground::SpotLightComponent::__ReflRegisterClass(registry);
	playground::StaticMeshComponent::__ReflRegisterClass(registry);

	// Registers every reflected global function in this project.
	

	registry.Finalize();
}

#endif // !defined(REFL_BUILD_REFLECTION)

#endif // !defined(REFL_GEN_DISABLE)
