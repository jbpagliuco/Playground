#include "Renderer.h"

#include "Core/Util/Color.h"

#include "Renderer/Material/Material.h"
#include "Renderer/Rect.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Scene/Camera.h"

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

	void Renderer::Present()
	{
		mSwapChain.Present();
	}

	const NGAPipelineState* Renderer::FindOrCreatePSO(const Material* material)
	{
		// Compute the checksum of the PSO.
		Checksum32 checksum = 0;
		checksum = crc32(material->GetShader(), checksum);
		checksum = crc32(material->GetShader()->GetVertexFormat(), checksum);

		// Check if this PSO already exists.
		if (mPSOs.find(checksum) != mPSOs.end()) {
			return &mPSOs[checksum];
		}

		// Create the new PSO.
		NGAPipelineStateDesc psoDesc;
		psoDesc.mVertexFormat = material->GetShader()->GetVertexFormat();
		psoDesc.mVertexShader = &material->GetShader()->GetVertexShader().GetShader();
		psoDesc.mPixelShader = &material->GetShader()->GetPixelShader().GetShader();

		NGAPipelineState& pso = mPSOs[checksum];
		const bool success = pso.Construct(psoDesc);
		CORE_ASSERT_RETURN_VALUE(success, nullptr, "Failed to construct PSO for material: %s", material->GetName());

		return &pso;
	}
}