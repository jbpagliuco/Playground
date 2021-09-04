#pragma once

#include "PhysX/include/foundation/PxTransform.h"

#include "Core/Math/Transform.h"

namespace playground
{
	physx::PxTransform GameTransformToPhysX(const Transform &transform);
	Transform PhysXTransformToGame(const physx::PxTransform &transform);
}