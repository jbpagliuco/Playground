#pragma once

#include "Core/Streaming/Stream.h"

#include "GameComponent.h"

namespace playground
{
	class Skybox;

	class SkyboxComponent : public GameComponentBase<GameComponentType::SKYBOX>
	{
	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		Skybox* GetSkybox();

	private:
		AssetID mSkyboxID;
	};
}