#include "PhysicsErrorHandler.h"

#include "Core/Debug/Assert.h"

namespace playground
{
	void PhysicsErrorHandler::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		CORE_ASSERT(false, "PhysX Error:\n\n(Code %d)\nMessage: %s\n\nFilename: %s\nLine: %d", (int)code, message, file, line);
	}
}