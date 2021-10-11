#pragma once

#include "Core/Streaming/Stream.h"

#include "GameComponent.h"

#include "SkyboxComponent.reflgen.h"

namespace playground
{
	class Skybox;

	class REFLECTED SkyboxComponent : public GameComponentBase<GameComponentType::SKYBOX>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void Activate() override;
		virtual void Deactivate() override;

		Skybox* GetSkybox();

	private:
		AssetID mSkyboxID REFLECTED REFL_NAME("skybox");
	};
}