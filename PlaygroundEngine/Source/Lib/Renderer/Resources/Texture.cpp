#include "Texture.h"

#include "DirectXTK/Include/DDSTextureLoader.h"

#include "Core/Util/String.h"
#include "Renderer/Renderer.h"

namespace playground
{
	bool Texture::Initialize(const TextureDesc& textureDesc, bool releaseTextureReference)
	{
		if (textureDesc.mTextureDesc.mBindFlags & NGA_TEXTURE_BIND_SHADER_RESOURCE) {
			bool success = mSampler.Construct(textureDesc.mSamplerStateDesc);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to construct sampler state for texture.");
		}

		bool success = mTexture.Construct(textureDesc.mTextureDesc, nullptr);
		RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to construct texture.");

		success = CreateViews(textureDesc);
		RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create texure views.");

		// We might not need to keep a reference to our texture.
		if (releaseTextureReference) {
			mTexture.Destruct();
		}

		return true;
	}

	bool Texture::Initialize(const TextureDesc &textureDesc, const std::string &filename, bool releaseTextureReference)
	{
		if (textureDesc.mTextureDesc.mBindFlags & NGA_TEXTURE_BIND_SHADER_RESOURCE) {
			bool success = mSampler.Construct(textureDesc.mSamplerStateDesc);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to construct sampler state for texture %s", filename.c_str());
		}

		bool success = mTexture.ConstructFromFile(textureDesc.mTextureDesc, filename.c_str());
		RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to construct texture %s", filename.c_str());

		success = CreateViews(textureDesc);
		RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create texure views for texture %s.", filename.c_str());

		// We might not need to keep a reference to our texture.
		if (releaseTextureReference) {
			mTexture.Destruct();
		}

		return true;
	}

	bool Texture::Initialize(const NGASwapChain& swapChain)
	{
		mRenderTargetViews = std::vector<NGARenderTargetView>(swapChain.GetNumBuffers());
		for (int i = 0; i < swapChain.GetNumBuffers(); ++i) {
			bool success = mRenderTargetViews[i].Construct(swapChain, i);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target view from swap chain buffer %d.", i);
		}

		return true;
	}

	void Texture::Shutdown()
	{
		mShaderResourceView.Destruct();

		for (auto& dsv : mDepthStencilViews) {
			dsv.Destruct();
		}

		for (auto& rtv : mRenderTargetViews) {
			rtv.Destruct();
		}

		mTexture.Destruct();
		mSampler.Destruct();
	}


	const NGATexture& Texture::GetResource()const
	{
		return mTexture;
	}

	const NGASamplerState& Texture::GetSamplerState()const
	{
		return mSampler;
	}

	const NGAShaderResourceView& Texture::GetShaderResourceView()const
	{
		return mShaderResourceView;
	}

	const NGARenderTargetView& Texture::GetRenderTargetView(int slice)const
	{
		if (slice >= mRenderTargetViews.size()) {
			return NGARenderTargetView::INVALID;
		}

		return mRenderTargetViews[slice];
	}

	const NGADepthStencilView& Texture::GetDepthStencilView(int slice)const
	{
		if (slice >= mDepthStencilViews.size()) {
			return NGADepthStencilView::INVALID;
		}

		return mDepthStencilViews[slice];
	}

	bool Texture::IsShaderResource()const
	{
		return mShaderResourceView != NGAShaderResourceView::INVALID;
	}


	bool Texture::CreateViews(const TextureDesc& textureDesc)
	{
		const bool isRenderTarget = textureDesc.mTextureDesc.mBindFlags & NGA_TEXTURE_BIND_RENDER_TARGET;
		const bool isDepthStencil = textureDesc.mTextureDesc.mBindFlags & NGA_TEXTURE_BIND_DEPTH_STENCIL;
		RENDER_ASSERT_RETURN_VALUE(!(isRenderTarget && isDepthStencil), "Texture cannot be bound as both render target and depth stencil view.");

		if (textureDesc.mTextureDesc.mBindFlags & NGA_TEXTURE_BIND_SHADER_RESOURCE) {
			bool success = mShaderResourceView.Construct(mTexture);
			RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create shader resource view.");
		}

		for (int i = 0; i < textureDesc.mTextureDesc.mArraySize; ++i) {
			if (isRenderTarget) {
				NGARenderTargetView& rtv = mRenderTargetViews.emplace_back();
				bool success = rtv.Construct(mTexture, i);
				RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create render target view.");
			}

			if (isDepthStencil) {
				NGADepthStencilView& dsv = mDepthStencilViews.emplace_back();
				bool success = dsv.Construct(mTexture, i);
				RENDER_ASSERT_RETURN_VALUE(success, false, "Failed to create depth stencil view.");
			}
		}

		return true;
	}
}