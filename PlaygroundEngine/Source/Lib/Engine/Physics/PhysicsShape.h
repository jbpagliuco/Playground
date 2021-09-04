#pragma once

#include "Core/Util/Util.h"

namespace playground
{
	enum class PhysicsShapeType
	{
		SPHERE = 0,
		BOX
	};

	struct PhysicsSphereInfo
	{
		float mRadius;
	};

	struct PhysicsBoxInfo
	{
		Vector3f mMin;
		Vector3f mMax;
	};

	struct PhysicsShapeInfo
	{
		PhysicsShapeType mType;

		union
		{
			PhysicsSphereInfo mSphereParams;
			PhysicsBoxInfo mBoxParams;
		};
	};
}