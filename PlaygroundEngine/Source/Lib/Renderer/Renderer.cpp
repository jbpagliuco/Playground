#include "Renderer.h"

#include "Core/Util/Color.h"

#include "Rect.h"
#include "Scene/Camera.h"
#include "RenderingSystem.h"

namespace playground
{
	CORE_CREATE_SINGLETON_INSTANCE(Renderer);

	bool Renderer::Initialize(const RendererInitParams &params)
	{
		mWindow = params.mWindow;
		
		NGASwapChainDesc swapChainDesc;
		swapChainDesc.mBufferCount = 2;
		swapChainDesc.mWindow = params.mWindow;
		bool success = mSwapChain.Construct(swapChainDesc);
		CORE_FATAL_ERROR(success, "Failed to construct swap chain.");

		RenderTargetTextureDesc depthBufferDesc;
		depthBufferDesc.mFormat = NGAFormat::D24_UNORM_S8_UINT;
		depthBufferDesc.mType = NGATextureType::TEXTURE2D;
		depthBufferDesc.mUsage = NGAUsage::GPU_WRITE;

		success = mMainRenderTarget.Initialize(mSwapChain, depthBufferDesc, params.mWidth, params.mHeight);
		CORE_FATAL_ERROR(success, "Failed to create main render target view.");

		// Register the back buffer render target as an engine resource
		RegisterEngineRenderTarget("backBuffer", &mMainRenderTarget);

		if (!mStateManager.Initialize()) {
			return false;
		}

		return true;
	}

	void Renderer::Shutdown()
	{
		mStateManager.Shutdown();
		mMainRenderTarget.Shutdown();
		mSwapChain.Destruct();
	}

	void Renderer::BeginRender()
	{
		Playground_RendererStateManager->OpenCommandList();
	}

	void Renderer::EndRender()
	{
		Playground_MainRenderTarget->Present(Playground_SwapChain->GetBufferIndex());
		Playground_RendererStateManager->CloseCommandList();

		mSwapChain.Present();
	}
}