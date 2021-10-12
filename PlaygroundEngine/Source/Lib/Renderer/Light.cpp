#include "Light.h"

#include <algorithm>
#include <vector>

#include "Core/Memory/Memory.h"

namespace playground
{
	Matrix Light::GetViewProj()const
	{
		Matrix lightView, lightProj;
		
		Vector lightDir(mDirection.x, mDirection.y, mDirection.z, 1.0f);
		Vector lightPos(mPosition.x, mPosition.y, mPosition.z, 1.0f);
		lightView = Matrix::LookAtLH(lightPos, lightPos + lightDir.V3Normalize(), Vector(0.0f, 1.0f, 0.0f, 0.0f));
		lightProj = Matrix::OrthoLH(20.0f, 20.0f, 0.1f, 100.0f);
		
		return lightProj * lightView;
	}
}