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

		CreateStaticSamplers();

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


	const NGAPipelineState* Renderer::FindOrCreatePSO(const NGAPipelineStateDesc& desc, const char* name)
	{
		// Compute the checksum of the PSO.
		Checksum32 checksum = 0;
		checksum = crc32(desc, checksum);

		// Check if this PSO already exists.
		if (mPSOs.find(checksum) != mPSOs.end()) {
			return &mPSOs[checksum];
		}

		NGAPipelineState& pso = mPSOs[checksum];
		const bool success = pso.Construct(desc);
		CORE_ASSERT_RETURN_VALUE(success, nullptr, "Failed to construct PSO for material: %s", name);

		return &pso;
	}

	const NGAPipelineState* Renderer::FindOrCreatePSO(const Material* material)
	{
		// Create the new PSO.
		NGAPipelineStateDesc psoDesc;
		psoDesc.mVertexFormat = material->GetShader()->GetVertexFormat();
		psoDesc.mVertexShader = &material->GetShader()->GetVertexShader().GetShader();
		psoDesc.mPixelShader = &material->GetShader()->GetPixelShader().GetShader();

		return FindOrCreatePSO(psoDesc, material->GetName().c_str());
	}

	bool Renderer::CreateStaticSamplers()
	{
		bool success = true;

		NGASamplerStateDesc wrapDesc;
		wrapDesc.mAddressU = NGATextureAddressMode::WRAP;
		wrapDesc.mAddressV = NGATextureAddressMode::WRAP;
		wrapDesc.mAddressW = NGATextureAddressMode::WRAP;

		NGASamplerStateDesc clampDesc;
		clampDesc.mAddressU = NGATextureAddressMode::CLAMP;
		clampDesc.mAddressV = NGATextureAddressMode::CLAMP;
		clampDesc.mAddressW = NGATextureAddressMode::CLAMP;

		// Point filters
		wrapDesc.mFilter = NGATextureFilter::MIN_MAG_MIP_POINT;
		success &= mStaticSamplers[(int)StaticSamplers::POINT_WRAP].Construct(wrapDesc);
		clampDesc.mFilter = NGATextureFilter::MIN_MAG_MIP_POINT;
		success &= mStaticSamplers[(int)StaticSamplers::POINT_CLAMP].Construct(clampDesc);

		// Linear filters
		wrapDesc.mFilter = NGATextureFilter::MIN_MAG_MIP_LINEAR;
		success &= mStaticSamplers[(int)StaticSamplers::LINEAR_WRAP].Construct(wrapDesc);
		clampDesc.mFilter = NGATextureFilter::MIN_MAG_MIP_LINEAR;
		success &= mStaticSamplers[(int)StaticSamplers::LINEAR_CLAMP].Construct(clampDesc);

		// Anisotropic filters
		wrapDesc.mFilter = NGATextureFilter::ANISOTROPIC;
		success &= mStaticSamplers[(int)StaticSamplers::ANISOTROPIC_WRAP].Construct(wrapDesc);
		clampDesc.mFilter = NGATextureFilter::ANISOTROPIC;
		success &= mStaticSamplers[(int)StaticSamplers::ANISOTROPIC_CLAMP].Construct(clampDesc);

		return success;
	}

	const NGASamplerState& Renderer::GetStaticSampler(StaticSamplers type)const
	{
		return mStaticSamplers[(int)type];
	}
}