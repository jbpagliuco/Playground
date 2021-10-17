#pragma once

#include "Core/Math/Transform.h"
#include "Core/Reflection/ReflMarkup.h"

#include "PhysicsShape.h"

namespace physx
{
	class PxRigidActor;
	class PxShape;
}

namespace playground
{
	class PhysicsMaterial;

	enum class REFLECTED PhysicsMotionType : uint8_t
	{
		STATIC,
		DYNAMIC
	};

	struct PhysicsRigidBodyInfo
	{
		PhysicsMotionType mMotionType;
		PhysicsShapeInfo mShapeInfo;
		PhysicsMaterial *mMaterial;
	};

	class PhysicsRigidBody
	{
	public:
		PhysicsRigidBody();
		virtual ~PhysicsRigidBody();

		bool Initialize(const PhysicsRigidBodyInfo &info, const Transform &transform);
		void Shutdown();

		Transform GetTransform()const;

	private:
		bool CreateShape(const PhysicsRigidBodyInfo &info);

	private:
		physx::PxRigidActor *mActor;
		physx::PxShape *mShape;
	};
}