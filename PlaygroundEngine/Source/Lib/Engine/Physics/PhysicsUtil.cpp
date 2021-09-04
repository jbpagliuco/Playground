#include "PhysicsUtil.h"

namespace playground
{
	physx::PxTransform GameTransformToPhysX(const Transform &transform)
	{
		const Vector3f pos = transform.mPosition.AsVector3();
		const Quaternion rot = transform.mRotation;

		return physx::PxTransform(
			physx::PxVec3(pos.x, pos.y, pos.z),
			physx::PxQuat(rot.x, rot.y, rot.z, rot.w)
		);
	}

	Transform PhysXTransformToGame(const physx::PxTransform &transform)
	{
		return Transform(
			Vector3f(transform.p.x, transform.p.y, transform.p.z),
			Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w),
			Vector3f(1.0f, 1.0f, 1.0f)
		);
	}
}