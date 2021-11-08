#pragma once

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)

#include "NGA/NGABlob.h"
#include "NGA/NGACommon.h"
#include "NGA/NGAShader.h"

namespace playground
{
	DXGI_FORMAT NGAFormatToDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToColorDXGI(NGAFormat format);
	DXGI_FORMAT NGATypelessFormatToDepthDXGI(NGAFormat format);

	NGAFormat DXGIFormatToNGA(DXGI_FORMAT format);

	D3D12_PRIMITIVE_TOPOLOGY_TYPE NGAPrimitiveTopologyToType(NGAPrimitiveTopology topology);

	// Compiles a shader.
	bool CompileHLSL(NGABlob& outBuffer, const char* filename, NGAShaderType type);

	UINT CalcConstantBufferByteSize(UINT byteSize);
}

#endif // CORE_RENDER_API(DX11)