#include "SceneEntity.h"

namespace playground
{
	SceneEntity::SceneEntity() :
		mPosition(0.0f, 0.0f, 0.0f),
		mRotation(Quaternion::Identity()),
		mDirty(false),
		mWorldTransform(Matrix::Identity())
	{
	}

	void SceneEntity::SetPosition(const Vector3f &position)
	{
		mDirty = true;
		mPosition = position;
	}

	Vector3f SceneEntity::GetPosition()const
	{
		return mPosition;
	}


	void SceneEntity::SetRotation(const Quaternion &rotation)
	{
		mDirty = true;
		mRotation = rotation;
	}

	Quaternion SceneEntity::GetRotation()const
	{
		return mRotation;
	}


	void SceneEntity::SetWorldTransform(const Matrix &worldTransform)
	{
		mDirty = false;
		mWorldTransform = worldTransform;
	}

	Matrix SceneEntity::GetWorldTransform()
	{
		if (mDirty) {
			mWorldTransform = Matrix::Translation(mPosition) * Matrix::RotationQuaternion(mRotation);
			mDirty = false;
		}

		return mWorldTransform;
	}
}