#pragma once

#include "Core/Math/Transform.h"
#include "Core/Reflection/ReflMarkup.h"
#include "Renderer/RenderDefs.h"

#include "Camera.reflgen.h"

namespace playground
{
	class RenderTarget;
	class Skybox;

	class REFLECTED Camera
	{
		GENERATED_REFLECTION_CODE();

	public:
		Camera() : 
			mEnabled(false),
			mFOV(0.0f),
			mNear(0.0f),
			mFar(0.0f),
			mRenderTarget(nullptr),
			mSkybox(nullptr),
			mId(NumCameras++) { }

		Matrix GetView()const;
		Matrix GetProj()const;
		Matrix GetViewProj()const;

	public:
		Transform mTransform;

		bool mEnabled				REFLECTED;
		float mFOV					REFLECTED REFL_ATTR_RADIANS;
		float mNear					REFLECTED;
		float mFar					REFLECTED;

		RenderTarget *mRenderTarget;
		Skybox* mSkybox;

		bool operator==(const Camera& other) { return other.mId == mId; }

	private:
		static int NumCameras;
		int mId;
	};
}