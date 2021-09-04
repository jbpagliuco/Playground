#pragma once

#include "Core/Core.h"

// Alignment
#if _MSC_VER && !__INTEL_COMPILER
#define CORE_ALIGN_MS(alignment) __declspec(align(alignment))
#else
#define CORE_ALIGN_MS(alignment)
#endif

#if __GNUC__
#define CORE_ALIGN_GCC(alignment) __attribute__((aligned(alignment)))
#else
#define CORE_ALIGN_GCC(alignment)
#endif

#define CORE_ALIGN(dec, align) CORE_ALIGN_MS(align) dec CORE_ALIGN_GCC(align)

#if CORE_CONFIG(DEBUG)
#define CORE_TRACK_MEMORY
#endif


#define CORE_ALLOC(size) playground::AllocateUnalignedMemory(size, __FILE__, __LINE__)
#define CORE_ALLOC_ALIGNED(size, alignment) playground::AllocateAlignedMemory(size, alignment, __FILE__, __LINE__)

#define CORE_FREE(p) playground::FreeUnalignedMemory(p);
#define CORE_FREE_ALIGNED(p) playground::FreeAlignedMemory(p);

namespace playground
{
	void MemorySystemShutdown();

	// Allocates memory on the heap.
	// @param sizeInBytes - The number of bytes to allocate.
	// @return A pointer to the beginning of the memory.
	void * AllocateUnalignedMemory(size_t sizeInBytes, const char *filename, int line, bool track = true, bool staticMemory = false);

	// Frees memory allocated by AllocateUnalignedMemory.
	// @param pMem - A pointer returned by AllocateUnalignedMemory.
	void FreeUnalignedMemory(void * pMem);

	// Allocates memory on the heap with an address alignment.
	// @param sizeInBytes - The number of bytes to allocate.
	// @param alignment - The alignment value, must be a power of two.
	// @return A pointer to the beginning of the aligned memory.
	void * AllocateAlignedMemory(size_t sizeInBytes, size_t alignment, const char *filename, int line, bool track = true, bool staticMemory = false);

	// Frees memory allocated by AllocateAlignedMemory.
	// @param pMem - A pointer returned by AllocateAlignedMemory.
	void FreeAlignedMemory(void * pMem);

#if defined(CORE_TRACK_MEMORY)
	size_t GetAllocationSize(void *pMem);
#endif
}