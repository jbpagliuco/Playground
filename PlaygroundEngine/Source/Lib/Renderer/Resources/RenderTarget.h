#pragma once

#include "Core/Streaming/AssetFactory.h"

#include "Renderer/Resources/Texture.h"

namespace playground
{
	struct RenderTargetTextureDesc
	{
		NGATextureType mType = NGATextureType::TEXTURE2D;
		NGAFormat mFormat = NGAFormat::UNKNOWN;
		NGAUsage mUsage = NGAUsage::GPU_WRITE;

		bool mShaderResource = false;
		int mArraySize = 1;

		NGASamplerStateDesc mSamplerStateDesc;

		operator TextureDesc()const;
	};

	struct RenderTargetDesc
	{
		int mWidth;
		int mHeight;

		bool mUseColorMap;
		bool mUseDepthMap;

		RenderTargetTextureDesc mColorMapDesc;
		RenderTargetTextureDesc mDepthMapDesc;
	};

	class RenderTarget : public AssetFactory<RenderTarget>
	{
	public:
		bool Initialize(RenderTargetDesc desc);
		bool Initialize(const NGASwapChain &swapChain, const RenderTargetTextureDesc &depthMapDesc, int width, int height);
		void Shutdown();

		const Texture& GetColorMap()const;
		const Texture& GetDepthMap()const;

		bool HasColorMap()const;
		bool HasDepthMap()const;

		void Bind(int slice = 0)const;
		void Clear(const float *clearColor, bool clearDepth, int slice = 0)const;

	private:
		Texture mColorMap;
		Texture mDepthMap;

		bool mHasColorMap;
		bool mHasDepthMap;
	};
}