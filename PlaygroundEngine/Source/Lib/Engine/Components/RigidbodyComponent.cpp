#include "RigidbodyComponent.h"

#include "PhysX/include/PxPhysics.h"
#include "PhysX/include/PxShape.h"
#include "PhysX/include/PxScene.h"
#include "PhysX/include/PxActor.h"
#include "PhysX/include/PxRigidDynamic.h"

#include "Core/Debug/Assert.h"

namespace playground
{
	void RigidbodyComponent::Deserialize(DeserializationParameterMap &params)
	{
		const std::string motionType = params["motionType"].AsString();
		if (motionType == "dynamic") {
			mMotionType = PhysicsMotionType::DYNAMIC;
		} else {
			mMotionType = PhysicsMotionType::STATIC;
		}

		const std::string collisionType = params["collisionType"].AsString();
		if (collisionType == "sphere") {
			mShapeInfo.mType = PhysicsShapeType::SPHERE;
			mShapeInfo.mSphereParams.mRadius = params["radius"].AsFloat();
		} else if (collisionType == "box") {
			mShapeInfo.mType = PhysicsShapeType::BOX;
			mShapeInfo.mBoxParams.mMin = params["min"].AsFloat3();
			mShapeInfo.mBoxParams.mMax = params["max"].AsFloat3();
		} else {
			CORE_ASSERT(false, "Unknown collision type given to rigidbody component (%s).", collisionType);
		}
	}

	void RigidbodyComponent::Activate()
	{
		mMaterial.Initialize();

		PhysicsRigidBodyInfo info;
		info.mMaterial = &mMaterial;
		info.mMotionType = mMotionType;
		info.mShapeInfo = mShapeInfo;

		mBody.Initialize(info, *mTransform);
	}

	void RigidbodyComponent::Deactivate()
	{
		mBody.Shutdown();
		mMaterial.Shutdown();
	}

	void RigidbodyComponent::UpdatePhysicsPost()
	{
		Transform newTr = mBody.GetTransform();

		mTransform->mPosition = newTr.mPosition;
		mTransform->mRotation = newTr.mRotation;
	}
}