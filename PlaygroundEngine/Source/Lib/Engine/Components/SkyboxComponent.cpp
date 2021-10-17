#include "SkyboxComponent.h"

#include "Renderer/Resources/Skybox.h"

#include "Engine/Components/CameraComponent.h"
#include "Engine/World/GameObject.h"

namespace playground
{
	void SkyboxComponent::Activate()
	{
		CameraComponent* camera = GetOwner()->GetComponentOfType<CameraComponent>();
		CORE_ASSERT(camera != nullptr, "Skybox component is on object with no camera component.");
	}

	void SkyboxComponent::Deactivate()
	{
		ReleaseAsset(mSkyboxID);
	}

	Skybox* SkyboxComponent::GetSkybox()
	{
		return Skybox::Get(mSkyboxID);
	}
}