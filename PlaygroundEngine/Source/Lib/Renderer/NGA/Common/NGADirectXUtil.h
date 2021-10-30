#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)

#include "NGA/NGACommon.h"
#include "NGA/NGAShader.h"

namespace playground
{
#if CORE_RENDER_API(DX11)
	typedef ID3D10Blob NGABlob;
#elif CORE_RENDER_API(DX12)
	typedef ID3DBlob NGABlob;
#endif


	DXGI_FORMAT NGAFormatToDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToColorDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToDepthDXGI(NGAFormat format);

	NGAFormat DXGIFormatToNGA(DXGI_FORMAT format);

	// Compiles a shader.
	bool CompileHLSL(NGABlob** outBuffer, const char* filename, NGAShaderType type);
}

#endif // CORE_RENDER_API(DX11)