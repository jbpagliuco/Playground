#pragma once

#include <stdint.h>

#include "Core/Reflection/ReflMarkup.h"

#include "Core/Math/Matrix.h"
#include "Core/Util/Util.h"

#include "Light.reflgen.h"

namespace playground
{
	enum class LightType : int32_t
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2
	};

	struct REFLECTED Light
	{
		GENERATED_REFLECTION_CODE();

		LightType mType REFLECTED;
		int32_t mEnabled REFLECTED;
		float mIntensity REFLECTED;
		int32_t mShadowCastingIndex REFLECTED;

		Tuple3<float> mPosition;
		float _padding1;

		Tuple3<float> mDirection;
		float mRadius REFLECTED;

		Tuple4<float> mColor;

		float mSpotLightAngle REFLECTED;
		float mConstantAttenuation REFLECTED;
		float mLinearAttenuation REFLECTED;
		float mQuadraticAttenuation REFLECTED;

		Matrix GetViewProj()const;
	};
}