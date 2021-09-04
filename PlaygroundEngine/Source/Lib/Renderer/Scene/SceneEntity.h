#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Quaternion.h"

namespace playground
{
	class SceneEntity
	{
	public:
		SceneEntity();

		void SetPosition(const Vector3f &position);
		Vector3f GetPosition()const;

		void SetRotation(const Quaternion &rotation);
		Quaternion GetRotation()const;

		void SetWorldTransform(const Matrix &worldTransform);
		Matrix GetWorldTransform();

	private:
		Vector3f mPosition;
		Quaternion mRotation;

		bool mDirty;
		Matrix mWorldTransform;
	};
}