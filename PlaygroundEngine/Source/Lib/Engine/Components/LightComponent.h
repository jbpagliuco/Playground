#pragma once

#include "GameComponent.h"

#include "LightComponent.reflgen.h"

namespace playground
{
	struct Light;

	template <GameComponentType type>
	class LightComponent : public GameComponentBase<type>
	{
	public:
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	protected:
		void SetLight(Light *pLight);
		
		Light *mLight;
	};

	class REFLECTED DirectionalLightComponent : public LightComponent<GameComponentType::DIRECTIONAL_LIGHT>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;
	};

	class REFLECTED PointLightComponent : public LightComponent<GameComponentType::POINT_LIGHT>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;
	};

	class REFLECTED SpotLightComponent : public LightComponent<GameComponentType::SPOT_LIGHT>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;
	};
}

#include "LightComponent.inl"