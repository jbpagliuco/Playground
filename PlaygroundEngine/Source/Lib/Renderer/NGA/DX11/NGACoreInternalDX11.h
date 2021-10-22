#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11)

#include <d3d11.h>

#include "NGA/NGACommon.h"
#include "NGA/NGAUtilDXGI.h"

namespace playground
{
	class NGAInternalStateDX11
	{
	public:
		ID3D11Device *mDevice;
		ID3D11DeviceContext *mContext;
	};

	D3D11_USAGE NGAUsageToD3D11(NGAUsage usage);

	extern NGAInternalStateDX11 NgaDx11State;
}

#endif // CORE_RENDER_API(DX11)