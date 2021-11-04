#pragma once

#include "Core/Streaming/AssetFactory.h"
#include "Core/Reflection/ReflMarkup.h"

#include "Renderer/Resources/Texture.h"

#include "RenderTarget.reflgen.h"

namespace playground
{
	struct REFLECTED RenderTargetTextureDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		NGATextureType mType				REFLECTED = NGATextureType::TEXTURE2D;
		NGAFormat mFormat					REFLECTED = NGAFormat::UNKNOWN;
		NGAUsage mUsage						REFLECTED = NGAUsage::GPU_WRITE;

		bool mShaderResource				REFLECTED = false;
		int mArraySize						REFLECTED = 1;

		NGASamplerStateDesc mSamplerStateDesc		REFLECTED;

		operator TextureDesc()const;
	};

	struct REFLECTED RenderTargetDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		int mWidth		REFLECTED;
		int mHeight		REFLECTED;

		RenderTargetTextureDesc mColorMapDesc		REFLECTED;
		RenderTargetTextureDesc mDepthMapDesc		REFLECTED;

		bool mUseColorMap		REFLECTED REFL_BOOL_ENABLE(mColorMapDesc) = false;
		bool mUseDepthMap		REFLECTED REFL_BOOL_ENABLE(mDepthMapDesc) = false;
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
		void Present(int slice = 0)const;

	private:
		Texture mColorMap;
		Texture mDepthMap;

		bool mHasColorMap;
		bool mHasDepthMap;
	};
}