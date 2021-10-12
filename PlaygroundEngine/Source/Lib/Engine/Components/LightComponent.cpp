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


	/*void DirectionalLightComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		Light *pLight = CreateLight(LightType::DIRECTIONAL);
		
		pLight->mEnabled = params["enabled"].AsBool(true);
		pLight->mColor = params["color"].AsColorTuple();
		pLight->mIntensity = params["intensity"].AsFloat(1.0f);
		pLight->mConstantAttenuation = params["constantAttenuation"].AsFloat();
		pLight->mLinearAttenuation = params["linearAttenuation"].AsFloat();
		pLight->mQuadraticAttenuation = params["quadraticAttenuation"].AsFloat();

		SetLight(pLight);
	}


	void PointLightComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		Light *pLight = CreateLight(LightType::POINT);

		pLight->mEnabled = params["enabled"].AsBool(true);
		pLight->mColor = params["color"].AsColorTuple();
		pLight->mIntensity = params["intensity"].AsFloat(1.0f);
		pLight->mRadius = params["radius"].AsFloat(15.0f);
		pLight->mConstantAttenuation = params["constantAttenuation"].AsFloat();
		pLight->mLinearAttenuation = params["linearAttenuation"].AsFloat();
		pLight->mQuadraticAttenuation = params["quadraticAttenuation"].AsFloat();

		SetLight(pLight);
	}


	void SpotLightComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		Light *pLight = CreateLight(LightType::SPOT);

		pLight->mEnabled = params["enabled"].AsBool(true);
		pLight->mColor = params["color"].AsColorTuple();
		pLight->mIntensity = params["intensity"].AsFloat(1.0f);
		pLight->mSpotLightAngle = params["spotLightAngle"].AsRadian(45.0f);
		pLight->mConstantAttenuation = params["constantAttenuation"].AsFloat();
		pLight->mLinearAttenuation = params["linearAttenuation"].AsFloat();
		pLight->mQuadraticAttenuation = params["quadraticAttenuation"].AsFloat();

		SetLight(pLight);
	}*/
}