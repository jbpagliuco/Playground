#pragma once

#include <string>

#include "Core/Streaming/AssetFactory.h"

#include "Renderer/NGA/NGAResources.h"
#include "Renderer/NGA/NGAResourceViews.h"
#include "Renderer/NGA/NGASamplerState.h"

namespace playground
{
	struct TextureDesc
	{
		NGATextureDesc mTextureDesc;
		NGASamplerStateDesc mSamplerStateDesc;
	};

	class Texture
	{
	public:
		bool Initialize(const TextureDesc& textureDesc, bool releaseTextureReference);
		bool Initialize(const TextureDesc& textureDesc, const std::string& filename, bool releaseTextureReference);
		bool Initialize(const NGASwapChain& swapChain);
		void Shutdown();

		const NGATexture& GetResource()const;
		const NGASamplerState& GetSamplerState()const;

		const NGAShaderResourceView& GetShaderResourceView()const;

		const NGARenderTargetView& GetRenderTargetView(int slice = 0)const;
		const NGADepthStencilView& GetDepthStencilView(int slice = 0)const;

		bool IsShaderResource()const;

	private:
		bool CreateViews(const TextureDesc& textureDesc);

	private:
		NGATexture mTexture;
		NGASamplerState mSampler;

		NGAShaderResourceView mShaderResourceView;

		// A texture can either have a render target view or a depth stencil view, but never both.
		std::vector<NGARenderTargetView> mRenderTargetViews;
		std::vector<NGADepthStencilView> mDepthStencilViews;
	};
}