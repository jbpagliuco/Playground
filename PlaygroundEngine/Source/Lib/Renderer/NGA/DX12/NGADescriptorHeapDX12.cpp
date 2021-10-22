#include "NGADescriptorHeapDX12.h"

#if CORE_RENDER_API(DX12)

#include "Core/Debug/Assert.h"

#include "NGA/DX12/NGACoreInternalDX12.h"

namespace playground
{
	bool NGAResourceDescriptorHeap::Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, int maxDescriptors)
	{
		mType = type;
		mMaxDescriptors = maxDescriptors;

		// Query descriptor size.
		mDescriptorSize = NgaDx12State.mDevice->GetDescriptorHandleIncrementSize(type);

		// Create heap.
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.NumDescriptors = maxDescriptors;
		heapDesc.Type = type;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;

		HRESULT hr = NgaDx12State.mDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mHeap));
		return SUCCEEDED(hr);
	}

	void NGAResourceDescriptorHeap::Shutdown()
	{
		COM_SAFE_RELEASE(mHeap);
	}

	NGACPUDescriptorHandle NGAResourceDescriptorHeap::Allocate()
	{
		CORE_FATAL_ERROR(mCurrentFreeIndex < mMaxDescriptors, "Ran out of descriptors for heap (%d)", mType);

		NGACPUDescriptorHandle handle;

		handle = (NGACPUDescriptorHandle)mHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += mDescriptorSize * mCurrentFreeIndex;

		++mCurrentFreeIndex;

		return handle;
	}
}

#endif