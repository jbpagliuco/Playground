#pragma once

#include "GameComponent.h"

#include "Engine/Physics/PhysicsRigidBody.h"
#include "Engine/Physics/PhysicsShape.h"
#include "Engine/Physics/PhysicsMaterial.h"

#include "RigidbodyComponent.reflgen.h"

namespace playground
{
	class REFLECTED RigidbodyComponent : public GameComponentBase<GameComponentType::RIGIDBODY>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;

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