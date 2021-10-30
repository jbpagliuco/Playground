#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX12)

#include <d3d12.h>

namespace playground
{
	class NGACPUDescriptorHandle : public D3D12_CPU_DESCRIPTOR_HANDLE
	{
	public:
		NGACPUDescriptorHandle() { ptr = 0; }
		explicit NGACPUDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& o) noexcept :
			D3D12_CPU_DESCRIPTOR_HANDLE(o)
		{}

		// Equality test.
		bool operator==(const NGACPUDescriptorHandle& other)const { return ptr == other.ptr; }
		bool operator!=(const NGACPUDescriptorHandle& other)const { return ptr != other.ptr; }

		// Clears this handle (does not free the descriptor from the heap).
		inline void Clear() { ptr = 0; }

		// Does this handle point to a valid descriptor?
		inline bool IsValid()const { return ptr != 0; }
	};

	class NGAResourceDescriptorHeap
	{
	public:
		bool Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, int maxDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		void Shutdown();

		// Allocate a new resource descriptor from the heap.
		NGACPUDescriptorHandle Allocate();

	public:
		UINT mDescriptorSize;
		ID3D12DescriptorHeap* mHeap;

	private:
		D3D12_DESCRIPTOR_HEAP_TYPE mType;
		int mMaxDescriptors;
		int mCurrentFreeIndex = 0;
	};
}

#endif