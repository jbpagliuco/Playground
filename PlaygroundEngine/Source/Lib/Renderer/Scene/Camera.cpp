#include "Camera.h"

#include "Renderer/Renderer.h"

namespace playground
{
	int Camera::NumCameras = 0;

	Matrix Camera::GetView()const
	{
		return mTransform.GetMatrix().Inverted();
	}

	Matrix Camera::GetProj()const
	{
		return Matrix::PerspectiveFOVLH(mFOV, Playground_Renderer->GetWindow().GetAspectRatio(), mNear, mFar);
	}

	Matrix Camera::GetViewProj()const
	{
		return GetProj() * GetView();
	}
}