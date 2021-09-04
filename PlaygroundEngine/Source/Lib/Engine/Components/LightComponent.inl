#include "Renderer/Light.h"
#include "Renderer/Scene/Scene.h"

namespace playground
{
	template <GameComponentType type>
	void LightComponent<type>::Deactivate()
	{
		Scene::Get()->RemoveLight(mLight);
		DestroyLight(mLight);
	}

	template <GameComponentType type>
	void LightComponent<type>::UpdateLate(float deltaTime)
	{
		// Update some of the light properties from our transform.
		mLight->mPosition = GameComponent::mTransform->mPosition.AsTuple3();

		const Vector zAxis(0.0f, 0.0f, 1.0f, 0.0f);
		mLight->mDirection = (GameComponent::mTransform->mRotation * zAxis).AsTuple3();
	}

	template <GameComponentType type>
	void LightComponent<type>::SetLight(Light *pLight)
	{
		mLight = pLight;

		Scene::Get()->AddLight(mLight);
	}
}