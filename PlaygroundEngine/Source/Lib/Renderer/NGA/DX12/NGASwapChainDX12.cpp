#include "NGA/NGASwapChain.h"

#if CORE_RENDER_API(DX12)

#include "NGACoreInternalDX12.h"

#include <dxgi1_4.h>

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGASwapChain);

	bool NGASwapChain::Construct(const NGASwapChainDesc& desc)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(desc.mBufferCount > 1 && desc.mBufferCount <= MAX_SWAP_CHAIN_BUFFERS, false);

		mNumBuffers = desc.mBufferCount;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = desc.mBufferCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferDesc.Width = desc.mWindow.width;
		swapChainDesc.BufferDesc.Height = desc.mWindow.height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.OutputWindow = desc.mWindow.handle;
		swapChainDesc.Windowed = true;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = 0;

		HRESULT hr = NgaDx12State.mDXGIFactory->CreateSwapChain(NgaDx12State.mCommandQueue, &swapChainDesc, &mSwapChain);
		CORE_FATAL_ERROR(SUCCEEDED(hr), "Failed to create D3D12 swap chain.");

#if RENDER_DEBUG_FEATURE(STORE_NAMES)
		SetDebugName(mSwapChain, "Swap Chain");
		for (int i = 0; i < desc.mBufferCount; ++i) {
			ID3D12Resource* backBuffer;
			hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
			if (SUCCEEDED(hr)) {
				char name[128];
				snprintf(name, sizeof(name), "Back Buffer %d", i);
				SetDebugName(backBuffer, name);
			}
			COM_SAFE_RELEASE(backBuffer);
		}
#endif

		return true;
	}

	void NGASwapChain::Destruct()
	{
		COM_SAFE_RELEASE(mSwapChain);
	}

	bool NGASwapChain::IsConstructed()const
	{
		return mSwapChain != nullptr;
	}

	void NGASwapChain::SetFullscreenState(bool fullscreen)const
	{
		mSwapChain->SetFullscreenState(fullscreen, nullptr);
	}

	void NGASwapChain::Present()
	{
		mSwapChain->Present(0, 0);

		mCurrentBackBufferIndex = (mCurrentBackBufferIndex == 0) ? 1 : 0;
	}
}

#endif // CORE_RENDER_API(DX11)