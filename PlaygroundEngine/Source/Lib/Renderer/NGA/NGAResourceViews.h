#pragma once

#include "NGA/NGACommon.h"

namespace playground
{
	class NGATexture;
	class NGASwapChain;

	class NGAShaderResourceView;
	class NGARenderTargetView;
	class NGADepthStencilView;

	class NGAShaderResourceView
	{
		NGA_GPU_CLASS(NGAShaderResourceView);

	public:
		bool Construct(const NGATexture &texture);
		void Destruct();

		bool IsConstructed()const;

		bool PointsToSameResource(const NGAShaderResourceView& view)const;
		bool PointsToSameResource(const NGARenderTargetView& view)const;
		bool PointsToSameResource(const NGADepthStencilView& view)const;

		bool operator==(const NGAShaderResourceView& other)const;
		bool operator!=(const NGAShaderResourceView& other)const;

		void GenerateMips()const;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11ShaderResourceView *mView = nullptr;
#endif

		friend class NGACommandContext;
		friend class NGARenderTargetView;
		friend class NGADepthStencilView;
	};


	class NGARenderTargetView
	{
		NGA_GPU_CLASS(NGARenderTargetView);

	public:
		NGARenderTargetView(NGARenderTargetView&& view) noexcept;

		bool Construct(const NGATexture &texture, int slice = 0);
		bool Construct(const NGASwapChain &swapChain);
		void Destruct();

		bool IsConstructed()const;

		bool PointsToSameResource(const NGAShaderResourceView& view)const;
		bool PointsToSameResource(const NGARenderTargetView& view)const;
		bool PointsToSameResource(const NGADepthStencilView& view)const;

		bool operator==(const NGARenderTargetView& other)const;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11RenderTargetView *mView = nullptr;
#endif

		friend class NGACommandContext;
		friend class NGAShaderResourceView;
		friend class NGADepthStencilView;
	};


	struct NGADepthStencilViewDesc
	{
		NGAFormat mFormat;
		int mWidth;
		int mHeight;
	};

	class NGADepthStencilView
	{
		NGA_GPU_CLASS(NGADepthStencilView);

	public:
		NGADepthStencilView(NGADepthStencilView&& view) noexcept;

		bool Construct(const NGATexture &texture, int slice = 0);
		void Destruct();

		bool IsConstructed()const;

		bool PointsToSameResource(const NGAShaderResourceView& view)const;
		bool PointsToSameResource(const NGARenderTargetView& view)const;
		bool PointsToSameResource(const NGADepthStencilView& view)const;

		bool operator==(const NGADepthStencilView& other)const;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11DepthStencilView *mView = nullptr;
#endif

		friend class NGACommandContext;
		friend class NGAShaderResourceView;
		friend class NGARenderTargetView;
	};
}