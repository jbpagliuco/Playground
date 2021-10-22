#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX12)

#include <d3d12.h>
#include <dxgi.h>

#include "NGA/DX12/NGADescriptorHeapDX12.h"
#include "NGA/NGACommon.h"
#include "NGA/NGAUtilDXGI.h"

namespace playground
{
	class NGAInternalStateDX12
	{
	public:
		ID3D12Device* mDevice;
		IDXGIFactory* mDXGIFactory;
		ID3D12Fence* mFence;

		// Command queue
		ID3D12CommandQueue* mCommandQueue;
		ID3D12CommandAllocator* mCommandAllocator;
		ID3D12GraphicsCommandList* mCommandList;

		// Descriptor heaps
		NGAResourceDescriptorHeap mRtvHeap;
		NGAResourceDescriptorHeap mDsvHeap;
		NGAResourceDescriptorHeap mCbvSrvHeap;
	};

	extern NGAInternalStateDX12 NgaDx12State;
}

#endif // CORE_RENDER_API(DX11)