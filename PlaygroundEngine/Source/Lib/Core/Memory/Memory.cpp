#include "Memory.h"

#include "Console/Console.h"
#include "Debug/Assert.h"
#include "Debug/Log.h"

#define MEMORY_LOG_FILTER "Memory"

#if defined(CORE_TRACK_MEMORY)
#include <map>
struct MemoryAllocation
{
	size_t size;
	const char *filename;
	int line;
	bool staticMemory;
};

static std::map<void*, MemoryAllocation> MemoryAllocations;
static size_t TotalBytesAllocated = 0;
static size_t TotalBytesAllocatedExtra = 0;
#endif


namespace playground
{
#if defined(CORE_TRACK_MEMORY)
	CONSOLE_COMMAND(show_memory)
	{
		for (auto &alloc : MemoryAllocations) {
			bool t = true;
		}
	}
#endif

	void MemorySystemShutdown()
	{
#if defined(CORE_TRACK_MEMORY)
		bool complain = false;
		for (auto &alloc : MemoryAllocations) {
			// Only complain if this isn't static memory (i.e. memory that's allocated once and then never deallocated)
			if (!alloc.second.staticMemory) {
				complain = true;
				LogError(MEMORY_LOG_FILTER, "Allocation of '%zu' bytes from '%s line %d' was never freed.", alloc.second.size, alloc.second.filename, alloc.second.line);
			}
		}
		CORE_ASSERT_RETURN(!complain, "Memory was leaked. Check logs.");

		CORE_ASSERT_RETURN(TotalBytesAllocated == 0);
		CORE_ASSERT_RETURN(TotalBytesAllocatedExtra == 0);
#endif
	}

	void * AllocateUnalignedMemory(size_t sizeInBytes, const char *filename, int line, bool track, bool staticMemory)
	{
		void *p = new unsigned char[sizeInBytes];
#if defined(CORE_TRACK_MEMORY)
		if (track) {
			MemoryAllocations[p] = { sizeInBytes, filename, line, staticMemory };
			TotalBytesAllocated += sizeInBytes;
		}
#endif
		return p;
	}

	void FreeUnalignedMemory(void * pMem)
	{
#if defined(CORE_TRACK_MEMORY)
		TotalBytesAllocated -= MemoryAllocations[pMem].size;
		MemoryAllocations.erase(pMem);
#endif
		delete[] pMem;
	}



	void * AllocateAlignedMemory(size_t sizeInBytes, size_t alignment, const char *filename, int line, bool track, bool staticMemory)
	{
		CORE_ASSERT_RETURN_VALUE(alignment >= 1, nullptr);
		CORE_ASSERT_RETURN_VALUE(alignment <= 128, nullptr);
		CORE_ASSERT_RETURN_VALUE((alignment & (alignment - 1)) == 0, nullptr); // Check to see if power of 2.

		size_t totalMemoryToAllocate = sizeInBytes + alignment;

		uintptr_t rawAddress = reinterpret_cast<uintptr_t>(AllocateUnalignedMemory(totalMemoryToAllocate, filename, line, track, staticMemory));

		size_t mask = (alignment - 1);
		uintptr_t offset = (rawAddress & mask);
		ptrdiff_t adjustment = alignment - offset;

		uintptr_t alignedAddress = rawAddress + adjustment;

		CORE_ASSERT_RETURN_VALUE(adjustment < 256, nullptr);
		unsigned char * pAlignedMemory = reinterpret_cast<unsigned char*>(alignedAddress);
		pAlignedMemory[-1] = static_cast<unsigned char>(adjustment); // We will always have at least an extra 
																	 // byte before the aligned address if the
																	 // asserts pass.

#if defined(CORE_TRACK_MEMORY)
		TotalBytesAllocatedExtra += alignment;
#endif
		return static_cast<void*>(pAlignedMemory);
	}


	static void* CalculateRawAddressFromAlignedMemory(void *pMem, ptrdiff_t &extra)
	{
		const unsigned char * pAlignedMem = reinterpret_cast<unsigned char*>(pMem);
		uintptr_t pAlignedAddress = reinterpret_cast<uintptr_t>(pMem);
		ptrdiff_t adjustment = static_cast<ptrdiff_t>(pAlignedMem[-1]);

		uintptr_t rawAddress = pAlignedAddress - adjustment;

		extra = pAlignedAddress - rawAddress;
		return reinterpret_cast<void*>(rawAddress);
	}

	void FreeAlignedMemory(void * pMem)
	{
		ptrdiff_t extra;
		void * pRawMem = CalculateRawAddressFromAlignedMemory(pMem, extra);

#if defined(CORE_TRACK_MEMORY)
		TotalBytesAllocatedExtra -= extra;
#endif

		FreeUnalignedMemory(pRawMem);
	}

#if defined(CORE_TRACK_MEMORY)
	size_t GetAllocationSize(void *pMem)
	{
		// Is the raw address being tracked?
		if (MemoryAllocations.find(pMem) != MemoryAllocations.end()) {
			return MemoryAllocations[pMem].size;
		}

		ptrdiff_t extra;
		void * pRawMem = CalculateRawAddressFromAlignedMemory(pMem, extra);
		CORE_ASSERT_RETURN_VALUE(MemoryAllocations.find(pRawMem) != MemoryAllocations.end(), 0, "Failed to find allocation at 0x%p", pMem);

		return MemoryAllocations[pRawMem].size;
	}
#endif
}