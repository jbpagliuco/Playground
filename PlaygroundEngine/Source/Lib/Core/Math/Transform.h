#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace playground
{
	class Transform
	{
	public:
		Transform();
		Transform(const Vector3f &position, const Quaternion &rotation, const Vector3f &scale);
		Transform(const Vector &position, const Quaternion &rotation, const Vector &scale);

		static Transform Identity();
		static Transform LookAt(const Vector3f &position, const Vector3f &target);

	public:
		Matrix GetMatrix()const;

	public:
		Vector mPosition;
		Quaternion mRotation;
		Vector mScale;
	};
}