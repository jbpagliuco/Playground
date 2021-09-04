#pragma once

namespace playground
{
	const float PI = 3.1415927410125732421875;
	const float PI_DIV2 = PI / 2.0f;
	const float PI_DIV3 = PI / 3.0f;
	const float PI_DIV4 = PI / 4.0f;
	const float PI_DIV6 = PI / 6.0f;
	const float PI_MUL2 = PI * 2.0f;
	const float PI_3DIV2 = 3.0f * PI / 2.0f;

	const float FLOAT_EPSILON = 1E-5f;
	const double DOUBLE_EPSILON = 1E-9;
	const long double LONGDOUBLE_EPSILON = DOUBLE_EPSILON;

	inline float ToRadians(float degrees) { return degrees * PI / 180.0f; }
	inline float ToDegrees(float radians) { return radians * 180.0f / PI; }

	// Single precision epsilon-equality.
	// Compares first and second, to see if they equal eachother within epsilon.
	inline bool FEquals(float first, float second, float epsilon = FLOAT_EPSILON) { return (fabsf(first - second) < epsilon); }

	// Double precision epsilon-equality.
	// Compares first and second, to see if they equal eachother within epsilon.
	inline bool DEquals(double first, double second, double epsilon = DOUBLE_EPSILON) { return (fabs(first - second) < epsilon); }
}