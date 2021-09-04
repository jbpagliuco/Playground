#include "Transform.h"

namespace playground
{
	Transform::Transform() :
		mPosition(Vector::Zero()),
		mRotation(Quaternion::Identity()),
		mScale(1.0f, 1.0f, 1.0f, 0.0f)
	{
	}

	Transform::Transform(const Vector3f &position, const Quaternion &rotation, const Vector3f &scale) :
		mPosition(position),
		mRotation(rotation),
		mScale(scale)
	{
	}

	Transform::Transform(const Vector &position, const Quaternion &rotation, const Vector &scale) :
		mPosition(position),
		mRotation(rotation),
		mScale(scale)
	{
	}
	
	Transform Transform::Identity()
	{
		return Transform();
	}

	Transform Transform::LookAt(const Vector3f &position, const Vector3f &target)
	{
		Transform tr;

		tr.mPosition = Vector(position);
		
		const Matrix rotation = Matrix::LookAtLH(tr.mPosition, Vector(target), Vector(0.0f, 1.0f, 0.0f, 0.0f));
		tr.mRotation = Quaternion::FromMatrix(rotation);

		tr.mScale = Vector(1.0f, 1.0f, 1.0f, 0.0f);

		return tr;
	}

	Matrix Transform::GetMatrix()const
	{
		return Matrix::Translation(mPosition) * Matrix::RotationQuaternion(mRotation) * Matrix::Scaling(mScale);
	}
}