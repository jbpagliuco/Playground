#pragma once

#include "GameComponent.h"

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

	class DirectionalLightComponent : public LightComponent<GameComponentType::DIRECTIONAL_LIGHT>
	{
	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;
	};

	class PointLightComponent : public LightComponent<GameComponentType::POINT_LIGHT>
	{
	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;
	};

	class SpotLightComponent : public LightComponent<GameComponentType::SPOT_LIGHT>
	{
	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;
	};
}

#include "LightComponent.inl"