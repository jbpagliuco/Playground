#pragma once

#include "GameComponent.h"

#include "Core/Reflection/ReflMarkup.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Scene/Camera.h"

#include "CameraComponent.reflgen.h"

namespace playground
{
	class REFLECTED CameraComponent : public GameComponentBase<GameComponentType::CAMERA>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	private:
		Camera mCamera				REFLECTED;
		AssetID mRenderTargetID		REFLECTED REFL_NAME("renderTarget") = INVALID_ASSET_ID;
	};
}