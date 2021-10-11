#pragma once

#include "GameComponent.h"

#include "Renderer/Resources/Texture.h"
#include "Renderer/Scene/Camera.h"

#include "BasicMovementComponent.reflgen.h"

namespace playground
{
	class REFLECTED BasicMovementComponent : public GameComponentBase<GameComponentType::BASIC_MOVEMENT>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;

		virtual void Activate() override;

		virtual void Update(float deltaTime) override;

	private:
		float mPitch;
		float mYaw;
		float mRoll;

		float mSensitivity;

		bool mStayFocused;
		Tuple3f mFocusPoint;
	};
}