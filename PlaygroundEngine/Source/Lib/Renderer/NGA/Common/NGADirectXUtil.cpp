#include "NGADirectXUtil.h"

#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)

#include <d3dcompiler.h>
#include <map>

#include "Core/File/File.h"
#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"
#include "Core/Util/Util.h"

namespace playground
{
	// Entry point function names for shaders
	static constexpr const char* SHADER_ENTRYPOINTS[] = {
		"vsMain",
		"psMain"
	};

	// Shader targets
	static constexpr const char* COMPILE_TARGETS[] = {
		"vs_5_0",
		"ps_5_0"
	};




	DXGI_FORMAT NGAFormatToDXGI(NGAFormat format)
	{
		static constexpr DXGI_FORMAT CONV[] = {
			// Color formats
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,

			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_R16G16_FLOAT,
			DXGI_FORMAT_R16G16B16A16_FLOAT,

			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_UINT,

			DXGI_FORMAT_BC1_UNORM,
			DXGI_FORMAT_B8G8R8A8_UNORM,

			// Depth formats
			DXGI_FORMAT_D16_UNORM,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D32_FLOAT,

			// Typeless formats
			DXGI_FORMAT_R24G8_TYPELESS,
			DXGI_FORMAT_R32_TYPELESS,
			DXGI_FORMAT_R32G32_TYPELESS,
			DXGI_FORMAT_R32G32B32_TYPELESS,
			DXGI_FORMAT_R32G32B32A32_TYPELESS,

			DXGI_FORMAT_UNKNOWN
		};

		static_assert(STATIC_ARRAY_SIZE(CONV) == (int)NGAFormat::SIZE);

		return CONV[(int)format];
	}

	DXGI_FORMAT NGATypelessFormatToColorDXGI(NGAFormat format)
	{
		static std::map<NGAFormat, DXGI_FORMAT> CONV = {
			{ NGAFormat::R24G8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, },
			{ NGAFormat::R32_TYPELESS, DXGI_FORMAT_R32_FLOAT },
			{ NGAFormat::R32G32_TYPELESS, DXGI_FORMAT_R32G32_FLOAT },
			{ NGAFormat::R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_FLOAT },
			{ NGAFormat::R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_FLOAT },
		};

		return CONV[format];
	}

	DXGI_FORMAT NGATypelessFormatToDepthDXGI(NGAFormat format)
	{
		static std::map<NGAFormat, DXGI_FORMAT> CONV = {
			{ NGAFormat::R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, },
			{ NGAFormat::R32_TYPELESS, DXGI_FORMAT_D32_FLOAT }
		};

		return CONV[format];
	}

	NGAFormat DXGIFormatToNGA(DXGI_FORMAT format)
	{
		static std::map<DXGI_FORMAT, NGAFormat> CONV = {
			{ DXGI_FORMAT_R32_FLOAT, NGAFormat::R32_FLOAT },
			{ DXGI_FORMAT_R32G32_FLOAT, NGAFormat::R32G32_FLOAT },
			{ DXGI_FORMAT_R32G32B32_FLOAT, NGAFormat::R32G32B32_FLOAT },
			{ DXGI_FORMAT_R32G32B32A32_FLOAT, NGAFormat::R32G32B32A32_FLOAT },

			{ DXGI_FORMAT_R16_FLOAT, NGAFormat::R16_FLOAT },
			{ DXGI_FORMAT_R16G16_FLOAT, NGAFormat::R16G16_FLOAT },
			{ DXGI_FORMAT_R16G16B16A16_FLOAT, NGAFormat::R16G16B16A16_FLOAT },

			{ DXGI_FORMAT_BC1_UNORM, NGAFormat::BC1_UNORM },
			{ DXGI_FORMAT_B8G8R8A8_UNORM, NGAFormat::B8G8R8A8_UNORM },

			{ DXGI_FORMAT_D16_UNORM, NGAFormat::D16_UNORM },
			{ DXGI_FORMAT_D24_UNORM_S8_UINT, NGAFormat::D24_UNORM_S8_UINT },
			{ DXGI_FORMAT_D32_FLOAT, NGAFormat::D32_FLOAT },

			{ DXGI_FORMAT_R24G8_TYPELESS, NGAFormat::R24G8_TYPELESS },
			{ DXGI_FORMAT_R32_TYPELESS, NGAFormat::R32_TYPELESS },
			{ DXGI_FORMAT_R32G32_TYPELESS, NGAFormat::R32G32_TYPELESS },
			{ DXGI_FORMAT_R32G32B32_TYPELESS, NGAFormat::R32G32B32_TYPELESS },
			{ DXGI_FORMAT_R32G32B32A32_TYPELESS, NGAFormat::R32G32B32A32_TYPELESS },

			{ DXGI_FORMAT_UNKNOWN, NGAFormat::UNKNOWN },
		};

		CORE_ASSERT_RETURN_VALUE(CONV.find(format) != CONV.end(), NGAFormat::UNKNOWN, "Unknown DXGI format.");

		return CONV[format];
	}

	




	class ShaderProgramInclude : public ID3DInclude
	{
		virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFilename, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
		{
			std::string fullPath = "";
			switch (IncludeType)
			{
			case D3D_INCLUDE_LOCAL:
			case D3D_INCLUDE_SYSTEM:
			{
				const bool found = FindDataFileRecursively(fullPath, pFilename);
				if (!found) {
					CORE_ASSERT(false, "Failed to find HLSL include '%s'", pFilename);
					*ppData = nullptr;
					*pBytes = 0;
					return S_FALSE;
				}
				break;
			}

			default:
				break;
			}

			File file(fullPath, std::ios::in);
			if (!file) {
				*ppData = nullptr;
				*pBytes = 0;
				return S_FALSE;
			}

			*ppData = nullptr;
			*pBytes = 0;

			std::string fileContents = file.ReadTextFile();

			const size_t fileSize = fileContents.size() * sizeof(char);
			char* buf = (char*)CORE_ALLOC(fileSize + 1);
			strncpy_s(buf, fileSize + 1, fileContents.c_str(), fileContents.size());

			*ppData = buf;
			*pBytes = (UINT)fileSize;

			return S_OK;
		}

		virtual HRESULT __stdcall Close(LPCVOID pData) override
		{
			// Here we must correctly free buffer created in Open.
			CORE_FREE((void*)pData);
			return S_OK;
		}

	};

	bool CompileHLSL(NGABlob** outBuffer, const char* filename, NGAShaderType type)
	{
		const char* entrypoint = SHADER_ENTRYPOINTS[(int)type];
		const char* target = COMPILE_TARGETS[(int)type];

		std::wstring wfile = StrToWideString(filename);

		ShaderProgramInclude includer;

		NGABlob* errorMessage = nullptr;
		
#if CORE_RENDER_API(DX11)
		HRESULT hr = D3DCompileFromFile(wfile.c_str(), nullptr, &includer, entrypoint, target, D3D10_SHADER_ENABLE_STRICTNESS, 0, outBuffer, &errorMessage);
#elif CORE_RENDER_API(DX12)
		HRESULT hr = D3DCompileFromFile(wfile.c_str(), nullptr, &includer, entrypoint, target, 0, 0, outBuffer, &errorMessage);
#endif

		if (FAILED(hr)) {
			if (errorMessage != nullptr) {
				CORE_ASSERT(false, "Failed to compile HLSL shader '%ls' with error message:\r\n\r\n%.*s", wfile.c_str(), errorMessage->GetBufferSize(), errorMessage->GetBufferPointer());
			}
			else {
				CORE_ASSERT(false, "Failed to compile HLSL shader '%ls' with unknown error", wfile.c_str());
			}
			return false;
		}

		return true;
	}

}

#endif // CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)