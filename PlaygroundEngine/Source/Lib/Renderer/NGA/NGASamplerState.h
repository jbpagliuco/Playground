#pragma once

#include "Core/Reflection/ReflMarkup.h"
#include "Core/Util/Color.h"
#include "Core/Util/Util.h"

#include "NGA/NGACommon.h"

#include "NGASamplerState.reflgen.h"

namespace playground
{
	struct REFLECTED NGASamplerStateDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		NGATextureFilter mFilter				REFLECTED = NGATextureFilter::MIN_MAG_MIP_LINEAR;

		NGATextureAddressMode mAddressU			REFLECTED = NGATextureAddressMode::WRAP;
		NGATextureAddressMode mAddressV			REFLECTED = NGATextureAddressMode::WRAP;
		NGATextureAddressMode mAddressW			REFLECTED = NGATextureAddressMode::WRAP;

		ColorF mBorderColor						REFLECTED = COLOR_RED;
	};

	class NGASamplerState
	{
		NGA_GPU_CLASS(NGASamplerState);

	public:
		bool Construct(const NGASamplerStateDesc &desc);
		void Destruct();

		bool IsConstructed()const;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11SamplerState* mSamplerState;
#elif CORE_RENDER_API(DX12)
	public:
		inline D3D12_STATIC_SAMPLER_DESC GetDesc()const { return mSamplerDesc; }
	private:
		D3D12_STATIC_SAMPLER_DESC mSamplerDesc;
		bool mConstructed = false;
#endif

		friend class NGACommandContext;
	};
}