#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11)

#include <d3d11.h>

#include "NGA/NGACommon.h"

namespace playground
{
	class NGAInternalStateDX11
	{
	public:
		ID3D11Device *mDevice;
		ID3D11DeviceContext *mContext;
	};

	DXGI_FORMAT NGAFormatToDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToColorDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToDepthDXGI(NGAFormat format);

	D3D11_USAGE NGAUsageToD3D11(NGAUsage usage);

	NGAFormat DXGIFormatToNGA(DXGI_FORMAT format);

	extern NGAInternalStateDX11 NgaDx11State;
}

#endif // CORE_RENDER_API(DX11)