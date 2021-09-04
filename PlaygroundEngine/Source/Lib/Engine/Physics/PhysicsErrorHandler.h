#pragma once

#include "PhysX/include/PxPhysicsAPI.h"

namespace playground
{
	class PhysicsErrorHandler : public physx::PxErrorCallback
	{
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
	};
}