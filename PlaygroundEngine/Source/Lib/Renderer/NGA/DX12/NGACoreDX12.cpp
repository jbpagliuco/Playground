#pragma once

#include "NGA/NGACore.h"

#if CORE_RENDER_API(DX12)

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <dxgi1_3.h>
#include <dxgidebug.h>

#include "Core/Debug/Assert.h"

#include "NGACoreInternalDX12.h"

namespace playground
{
	constexpr UINT NUM_RTV_DESCRIPTORS = 5;
	constexpr UINT NUM_DSV_DESCRIPTORS = 5;
	constexpr UINT NUM_CBV_SRV_UAV_DESCRIPTORS = 15;

	void NGAInitialize(const NGAInitParams& params)
	{
		// Enable debug layer.
#if CORE_CONFIG(DEBUG)
		ID3D12Debug* debugInterface;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		debugInterface->EnableDebugLayer();
		COM_SAFE_RELEASE(debugInterface);
#endif // CORE_CONFIG(DEBUG)

		// Create DXGI factory.
		CreateDXGIFactory1(IID_PPV_ARGS(&NgaDx12State.mDXGIFactory));

		// Create device.
		HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&NgaDx12State.mDevice));
		CORE_FATAL_ERROR(SUCCEEDED(hr), "Failed to create DirectX 12 device.");

		// Break on errors
#if CORE_CONFIG(DEBUG)
		ID3D12InfoQueue* infoQueue;
		NgaDx12State.mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue));
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		COM_SAFE_RELEASE(infoQueue);
#endif // CORE_CONFIG(DEBUG)

		// Create synchronization fence.
		hr = NgaDx12State.mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&NgaDx12State.mFence));
		CORE_FATAL_ERROR(SUCCEEDED(hr), "Failed to create fence.");

		// Query for 4X MSAA support.
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		NgaDx12State.mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
		CORE_ASSERT(msQualityLevels.NumQualityLevels > 0, "Unexpected MSAA quality level.");

		// Create command queue and command list.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		NgaDx12State.mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&NgaDx12State.mCommandQueue));
		NgaDx12State.mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&NgaDx12State.mCommandAllocator));
		NgaDx12State.mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, NgaDx12State.mCommandAllocator, nullptr, IID_PPV_ARGS(&NgaDx12State.mCommandList));
		NgaDx12State.mCommandList->Close();

		NgaDx12State.mRtvHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_RTV_DESCRIPTORS);
		NgaDx12State.mDsvHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, NUM_DSV_DESCRIPTORS);
		NgaDx12State.mCbvSrvUavHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, NUM_CBV_SRV_UAV_DESCRIPTORS, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	void NGAShutdown()
	{
		NgaDx12State.mCbvSrvUavHeap.Shutdown();
		NgaDx12State.mDsvHeap.Shutdown();
		NgaDx12State.mRtvHeap.Shutdown();

		COM_SAFE_RELEASE(NgaDx12State.mCommandList);
		COM_SAFE_RELEASE(NgaDx12State.mCommandAllocator);
		COM_SAFE_RELEASE(NgaDx12State.mCommandQueue);

		COM_SAFE_RELEASE(NgaDx12State.mFence);
		COM_SAFE_RELEASE(NgaDx12State.mDXGIFactory);
		COM_SAFE_RELEASE(NgaDx12State.mDevice);

		// Report live objects
#if CORE_CONFIG(DEBUG)
		IDXGIDebug1* dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
		COM_SAFE_RELEASE(dxgiDebug);
#endif // CORE_CONFIG(DEBUG)
	}
}

#endif // CORE_RENDER_API(DX12)