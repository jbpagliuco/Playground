#pragma once

#include "Core/Math/Transform.h"
#include "Renderer/RenderDefs.h"

namespace playground
{
	class RenderTarget;
	class Skybox;

	class Camera
	{
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
		bool mEnabled;

		float mFOV;
		float mNear;
		float mFar;

		RenderTarget *mRenderTarget;
		Skybox* mSkybox;

		bool operator==(const Camera& other) { return other.mId == mId; }

	private:
		static int NumCameras;
		int mId;
	};
}