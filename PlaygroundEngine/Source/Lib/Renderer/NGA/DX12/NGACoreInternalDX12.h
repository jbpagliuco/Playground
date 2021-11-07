#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX12)

#include "DirectX/d3d12.h"
#include <dxgi.h>
#include "DirectX/d3dx12.h"
#include <d3dcompiler.h>

#include "NGA/Common/NGADirectXUtil.h"
#include "NGA/DX12/NGADescriptorHeapDX12.h"
#include "NGA/NGACommon.h"

namespace playground
{
	class NGAInternalStateDX12
	{
	public:
		ID3D12Device* mDevice;
		IDXGIFactory* mDXGIFactory;

		ID3D12Fence* mFence;
		size_t mCurrentFence = 0;

		// Command queue
		ID3D12CommandQueue* mCommandQueue;
		ID3D12CommandAllocator* mCommandAllocator;
		ID3D12GraphicsCommandList* mCommandList;

		// Descriptor heaps
		NGAResourceDescriptorHeap mRtvHeap;
		NGAResourceDescriptorHeap mDsvHeap;
		NGAResourceDescriptorHeap mCbvSrvUavHeap;
	};

#if RENDER_DEBUG_FEATURE(STORE_NAMES)
	void SetDebugName(IDXGIObject* object, const char* name);
	void SetDebugName(ID3D12Object* object, const char* name);
#else
	void SetDebugName(IDXGIObject* object, const char* name) {}
	void SetDebugName(ID3D12Object* object, const char* name) {}
#endif

	extern NGAInternalStateDX12 NgaDx12State;
}

#endif // CORE_RENDER_API(DX11)