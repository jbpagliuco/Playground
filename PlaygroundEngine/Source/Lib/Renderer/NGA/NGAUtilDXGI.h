#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)

#include "NGA/NGACommon.h"

namespace playground
{
	DXGI_FORMAT NGAFormatToDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToColorDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToDepthDXGI(NGAFormat format);

	NGAFormat DXGIFormatToNGA(DXGI_FORMAT format);
}

#endif // CORE_RENDER_API(DX11)