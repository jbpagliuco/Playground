#pragma once

#include "PhysicsDefs.h"

namespace physx
{
	class PxPhysics;
	class PxScene;
}

namespace playground
{
	bool PhysicsSystemInit();
	void PhysicsSystemShutdown();

	void PhysicsSystemDoFrame(float DeltaTime);

	physx::PxPhysics* PhysicsGetDevice();
	physx::PxScene* PhysicsGetScene();
}