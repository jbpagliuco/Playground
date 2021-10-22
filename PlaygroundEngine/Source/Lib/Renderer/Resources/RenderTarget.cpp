#include "RenderTarget.h"

#include "Renderer/NGA/NGASwapChain.h"
#include "Renderer/RenderDefs.h"
#include "Renderer/Renderer.h"

namespace playground
{
	CORE_FACTORY_SETUP(RenderTarget);

	RenderTargetTextureDesc::operator TextureDesc()const
	{
		TextureDesc desc;
		desc.mTextureDesc.mFormat = mFormat;
		desc.mTextureDesc.mType = mType;
		desc.mTextureDesc.mUsage = mUsage;
		desc.mTextureDesc.mBindFlags = (mShaderResource) ? NGA_TEXTURE_BIND_SHADER_RESOURCE : NGA_TEXTURE_BIND_NONE;
		desc.mTextureDesc.mArraySize = mArraySize;
		desc.mSamplerStateDesc = mSamplerStateDesc;

		return desc;
	}

	bool RenderTarget::Initialize(RenderTargetDesc desc)
	{
		mHasColorMap = desc.mUseColorMap;
		mHasDepthMap = desc.mUseDepthMap;

		if (desc.mUseColorMap) {
			TextureDesc colorMapDesc;
			colorMapDesc = desc.mColorMapDesc;
			colorMapDesc.mTextureDesc.mWidth = desc.mWidth;
			colorMapDesc.mTextureDesc.mHeight = desc.mHeight;
			colorMapDesc.mTextureDesc.mBindFlags |= NGA_TEXTURE_BIND_RENDER_TARGET;

			bool success = mColorMap.Initialize(colorMapDesc, true);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target color map.");
		}

		if (desc.mUseDepthMap) {
			TextureDesc depthMapDesc;
			depthMapDesc = desc.mDepthMapDesc;
			depthMapDesc.mTextureDesc.mWidth = desc.mWidth;
			depthMapDesc.mTextureDesc.mHeight = desc.mHeight;
			depthMapDesc.mTextureDesc.mBindFlags |= NGA_TEXTURE_BIND_DEPTH_STENCIL;

			bool success = mDepthMap.Initialize(depthMapDesc, true);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target depth map.");
		}

		return true;
	}

	bool RenderTarget::Initialize(const NGASwapChain& swapChain, const RenderTargetTextureDesc& depthMapDesc, int width, int height)
	{
		mHasColorMap = true;
		mHasDepthMap = true;

		bool success = mColorMap.Initialize(swapChain);
		RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target color map.");

		TextureDesc desc = depthMapDesc;
		desc.mTextureDesc.mBindFlags |= NGA_TEXTURE_BIND_DEPTH_STENCIL;
		desc.mTextureDesc.mWidth = width;
		desc.mTextureDesc.mHeight = height;

		//success = mDepthMap.Initialize(desc, true);
		//RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target depth map.");

		return true;
	}

	void RenderTarget::Shutdown()
	{
		mColorMap.Shutdown();
		mDepthMap.Shutdown();
	}

	const Texture& RenderTarget::GetColorMap()const
	{
		return mColorMap;
	}

	const Texture& RenderTarget::GetDepthMap()const
	{
		return mDepthMap;
	}

	bool RenderTarget::HasColorMap()const
	{
		return mHasColorMap;
	}

	bool RenderTarget::HasDepthMap()const
	{
		return mHasDepthMap;
	}

	void RenderTarget::Bind(int slice)const
	{
		Playground_RendererStateManager->BindRenderTarget(mColorMap.GetRenderTargetView(slice), mDepthMap.GetDepthStencilView(slice));
	}
	
	void RenderTarget::Clear(const float* clearColor, bool clearDepth, int slice)const
	{
		if (HasColorMap()) {
			Playground_RendererStateManager->ClearRenderTarget(GetColorMap().GetRenderTargetView(slice), clearColor);
		}

		if (clearDepth && HasDepthMap()) {
			Playground_RendererStateManager->ClearDepthStencilView(GetDepthMap().GetDepthStencilView(slice));
		}
	}
}