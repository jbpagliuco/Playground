#pragma once

#include "GameComponent.h"

#include "Engine/Physics/PhysicsRigidBody.h"
#include "Engine/Physics/PhysicsShape.h"
#include "Engine/Physics/PhysicsMaterial.h"

namespace playground
{
	class RigidbodyComponent : public GameComponentBase<GameComponentType::RIGIDBODY>
	{
	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdatePhysicsPost() override;

	private:
		PhysicsMotionType mMotionType;
		PhysicsShapeInfo mShapeInfo;

		PhysicsMaterial mMaterial;
		PhysicsRigidBody mBody;
	};
}