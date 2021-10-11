#pragma once

#include "GameComponent.h"

#include "Renderer/Resources/Texture.h"
#include "Renderer/Scene/Camera.h"

#include "CameraComponent.reflgen.h"

namespace playground
{
	class REFLECTED CameraComponent : public GameComponentBase<GameComponentType::CAMERA>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	private:
		Camera mCamera;
		AssetID mRenderTargetID = INVALID_ASSET_ID;
	};
}