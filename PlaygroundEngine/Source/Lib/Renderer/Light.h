#pragma once

#include <stdint.h>

#include "Core/Reflection/ReflMarkup.h"

#include "Core/Math/Matrix.h"
#include "Core/Util/Color.h"
#include "Core/Util/Util.h"

#include "Light.reflgen.h"

namespace playground
{
	enum class REFLECTED LightType : int8_t
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT = 2
	};

	struct REFLECTED Light
	{
		GENERATED_REFLECTION_CODE();

		int32_t mType;
		int32_t mEnabled					REFLECTED = 1;
		float mIntensity					REFLECTED = 1.0f;
		int32_t mShadowCastingIndex;

		Tuple3<float> mPosition;

		LightType mLightType				REFLECTED;
		int8_t _padding3;
		int16_t _padding2;

		Tuple3<float> mDirection;
		float mRadius						REFLECTED = 15.0f;

		ColorF mColor						REFLECTED;

		float mSpotLightAngle				REFLECTED = 45.0f;
		float mConstantAttenuation			REFLECTED;
		float mLinearAttenuation			REFLECTED;
		float mQuadraticAttenuation			REFLECTED;

		Matrix GetViewProj()const;
	};
}