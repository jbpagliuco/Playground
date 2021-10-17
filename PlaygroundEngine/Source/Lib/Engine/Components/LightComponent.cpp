#include "LightComponent.h"

#include "Core/Math/Transform.h"
#include "Renderer/Scene/Scene.h"
#include "Engine/Input/Input.h"

namespace playground
{
	void LightComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		// Weirdness 
		mLight.mType = (int32_t)mLight.mLightType;
	}

	void LightComponent::Activate()
	{
		Scene::Get()->AddLight(&mLight);
	}

	void LightComponent::Deactivate()
	{
		Scene::Get()->RemoveLight(&mLight);
	}

	void LightComponent::UpdateLate(float deltaTime)
	{
		// Update some of the light properties from our transform.
		mLight.mPosition = GameComponent::mTransform->mPosition.AsTuple3();

		static const Vector zAxis(0.0f, 0.0f, 1.0f, 0.0f);
		mLight.mDirection = (GameComponent::mTransform->mRotation * zAxis).AsTuple3();
	}
}