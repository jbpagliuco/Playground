#pragma once

#include "PhysX/include/PxPhysicsAPI.h"

#include "Core/Memory/Memory.h"

namespace playground
{
	class PhysicsAllocator : public physx::PxAllocatorCallback
	{
	public:
		virtual ~PhysicsAllocator() {}
		virtual void* allocate(size_t size, const char *typeName, const char *filename, int line) override;
		virtual void deallocate(void* ptr) override;

#if defined(PLAYGROUND_PHYSICS_TRACK_MEMORY)
		inline size_t GetBytesAllocated()const { return mAllocated; }
	private:
		size_t mAllocated = 0;
#endif
	};
}