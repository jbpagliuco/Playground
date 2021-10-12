#pragma once

#include "Core/Reflection/ReflMarkup.h"
#include "Core/Math/Vector.h"
#include "Core/Util/Util.h"

#include "PhysicsShape.reflgen.h"

namespace playground
{
	enum class REFLECTED PhysicsShapeType : uint8_t
	{
		SPHERE = 0,
		BOX
	};

	struct REFLECTED PhysicsSphereInfo
	{
		GENERATED_REFLECTION_CODE();

	public:
		float mRadius			REFLECTED;
	};

	struct REFLECTED PhysicsBoxInfo
	{
		GENERATED_REFLECTION_CODE();

	public:
		Vector3f mMin			REFLECTED;
		Vector3f mMax			REFLECTED;
	};

	struct REFLECTED PhysicsShapeInfo
	{
		GENERATED_REFLECTION_CODE();

	public:
		PhysicsShapeType mType			REFLECTED;

		PhysicsSphereInfo mSphereParams		REFLECTED;
		PhysicsBoxInfo mBoxParams			REFLECTED;
	};
}