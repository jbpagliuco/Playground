#include "LightComponent.h"

#include "Core/Math/Transform.h"
#include "Renderer/Light.h"
#include "Renderer/Scene/Scene.h"
#include "Engine/Input/Input.h"

namespace playground
{
	void DirectionalLightComponent::DeserializePost(const DeserializationParameterMap& params)
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
	}
}