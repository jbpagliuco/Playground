#include "PhysicsMemory.h"

namespace playground
{
	static constexpr size_t PHYSX_MEM_ALIGN = 16;

	void* PhysicsAllocator::allocate(size_t size, const char *typeName, const char *filename, int line)
	{
#if defined(PLAYGROUND_PHYSICS_TRACK_MEMORY)
		mAllocated += size;
#endif

		return AllocateAlignedMemory(size, PHYSX_MEM_ALIGN, filename, line, true, false);
	}

	void PhysicsAllocator::deallocate(void *ptr)
	{
#if defined(PLAYGROUND_PHYSICS_TRACK_MEMORY)
		mAllocated -= GetAllocationSize(ptr);
#endif

		CORE_FREE_ALIGNED(ptr);
	}
}