#include "PhysicsRigidBody.h"

#include "PhysX/include/PxPhysics.h"
#include "PhysX/include/PxActor.h"
#include "PhysX/include/PxRigidDynamic.h"
#include "PhysX/include/PxRigidStatic.h"
#include "PhysX/include/PxShape.h"
#include "PhysX/include/PxScene.h"

#include "Core/Debug/Assert.h"

#include "Physics.h"
#include "PhysicsMaterial.h"
#include "PhysicsShape.h"
#include "PhysicsUtil.h"

namespace playground
{
	PhysicsRigidBody::PhysicsRigidBody()
	{
	}

	PhysicsRigidBody::~PhysicsRigidBody()
	{
	}

	bool PhysicsRigidBody::Initialize(const PhysicsRigidBodyInfo &info, const Transform &transform)
	{
		const bool success = CreateShape(info);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create physics shape.");

		const physx::PxTransform pxTr = GameTransformToPhysX(transform);

		switch (info.mMotionType)
		{
		case PhysicsMotionType::DYNAMIC:
			mActor = (physx::PxRigidActor*)(PhysicsGetDevice()->createRigidDynamic(pxTr));
			break;

		case PhysicsMotionType::STATIC:
			mActor = (physx::PxRigidActor*)(PhysicsGetDevice()->createRigidStatic(pxTr));
			break;
		};

		CORE_ASSERT_RETURN_VALUE(mActor, false, "Failed to create physics actor.");

		mActor->attachShape(*mShape);

		PhysicsGetScene()->addActor(*mActor);

		return true;
	}

	void PhysicsRigidBody::Shutdown()
	{
		PhysicsGetScene()->removeActor(*mActor);

		PLAYGROUND_PHYSICS_SAFE_RELEASE(mActor);
		PLAYGROUND_PHYSICS_SAFE_RELEASE(mShape);
	}

	Transform PhysicsRigidBody::GetTransform()const
	{
		return PhysXTransformToGame(mActor->getGlobalPose());
	}

	bool PhysicsRigidBody::CreateShape(const PhysicsRigidBodyInfo &info)
	{
		switch (info.mShapeInfo.mType)
		{
		case PhysicsShapeType::SPHERE:
		{
			physx::PxSphereGeometry geo;
			geo.radius = info.mShapeInfo.mSphereParams.mRadius;
			CORE_ASSERT_RETURN_VALUE(geo.isValid(), false, "Tried to create an invalid sphere.");

			mShape = PhysicsGetDevice()->createShape(geo, *info.mMaterial->GetPhysXMaterial());
			break;
		}

		case PhysicsShapeType::BOX:
		{
			PhysicsBoxInfo params = info.mShapeInfo.mBoxParams;

			physx::PxBoxGeometry geo;
			geo.halfExtents.x = (params.mMax.x - params.mMin.x) / 2.0f;
			geo.halfExtents.y = (params.mMax.y - params.mMin.y) / 2.0f;
			geo.halfExtents.z = (params.mMax.z - params.mMin.z) / 2.0f;
			CORE_ASSERT_RETURN_VALUE(geo.isValid(), false, "Tried to create an invalid box.");

			mShape = PhysicsGetDevice()->createShape(geo, *info.mMaterial->GetPhysXMaterial());
			break;
		}
		};

		return mShape != nullptr;
	}
}