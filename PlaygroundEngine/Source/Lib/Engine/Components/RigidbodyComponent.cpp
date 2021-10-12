#include "RigidbodyComponent.h"

#include "PhysX/include/PxPhysics.h"
#include "PhysX/include/PxShape.h"
#include "PhysX/include/PxScene.h"
#include "PhysX/include/PxActor.h"
#include "PhysX/include/PxRigidDynamic.h"

#include "Core/Debug/Assert.h"

namespace playground
{
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