#pragma once

#include "Core/Util/Util.h"

#include "NGA/NGACommon.h"

namespace playground
{
	struct NGASamplerStateDesc
	{
		NGATextureFilter mFilter = NGATextureFilter::MIN_MAG_MIP_LINEAR;

		NGATextureAddressMode mAddressU = NGATextureAddressMode::WRAP;
		NGATextureAddressMode mAddressV = NGATextureAddressMode::WRAP;
		NGATextureAddressMode mAddressW = NGATextureAddressMode::WRAP;

		Tuple4f mBorderColor = Tuple4f(1.0f, 0.0f, 0.0f, 1.0f);
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
		struct ID3D11SamplerState *mSamplerState;
#endif

		friend class NGACommandContext;
	};
}