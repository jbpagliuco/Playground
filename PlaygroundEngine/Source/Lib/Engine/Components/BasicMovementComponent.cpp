#include "BasicMovementComponent.h"

#include "Engine/Input/Input.h"
#include "Engine/World/GameObject.h"
#include "Renderer/Renderer.h"
#include "Renderer/Scene/Scene.h"

namespace playground
{
	void BasicMovementComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		mStayFocused = false;
		if (params.HasChild("focus")) {
			mStayFocused = true;
			mFocusPoint = params["focus"].AsTuple3f();
		}
	}

	void BasicMovementComponent::Activate()
	{
		mTransform->GetMatrix().ExtractAngles(mPitch, mYaw, mRoll);
	}

	void BasicMovementComponent::Update(float deltaTime)
	{
		mYaw += GetMouseDelta().x * deltaTime * mSensitivity;
		mPitch += GetMouseDelta().y * deltaTime * mSensitivity;

		const float baseSpeed = 3.0f;
		const float fastSpeed = 3.0f;
		const float fastModifier = (IsShiftDown() || IsKeyDown('F')) ? fastSpeed : 1.0f;

		Vector3f delta;
		delta.x = (IsKeyDown('D') - IsKeyDown('A')) * deltaTime * baseSpeed * fastModifier;
		delta.y = (IsKeyDown('E') - IsKeyDown('Q')) * deltaTime * baseSpeed * fastModifier;
		delta.z = (IsKeyDown('W') - IsKeyDown('S')) * deltaTime * baseSpeed * fastModifier;

		mTransform->mRotation = Quaternion::FromEuler(mPitch, mYaw, mRoll);
		mTransform->mPosition = Vector(mTransform->mPosition.AsVector3() + (mTransform->mRotation * delta));
		
		if (mStayFocused) {
			Matrix m = Matrix::LookAtLH(mTransform->mPosition, Vector(Vector3f(mFocusPoint.vArray)), Vector(0.0f, 1.0f, 0.0f, 0.0f));
			mTransform->mRotation = Quaternion::FromMatrix(m);
		}
	}
}