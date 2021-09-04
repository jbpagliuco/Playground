#include "NGA/NGAShader.h"

#if CORE_RENDER_API(DX11)

#include <string>

#include <d3dcompiler.h>

#include "Core/File/File.h"
#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"
#include "NGACoreInternalDX11.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAShader);

	static constexpr const char *SHADER_ENTRYPOINTS[] = {
		"vsMain",
		"psMain"
	};

	static constexpr const char *COMPILE_TARGETS[] = {
		"vs_5_0",
		"ps_5_0"
	};

	class ShaderProgramInclude : public ID3DInclude
	{
		virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFilename, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override
		{
			std::string fullPath = "";
			switch(IncludeType) 
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

	static bool CompileShader(ID3D10Blob **outBuffer, const std::string &file, NGAShaderType type)
	{
		const char *entrypoint = SHADER_ENTRYPOINTS[(int)type];
		const char *target = COMPILE_TARGETS[(int)type];
		
		std::wstring wfile = StrToWideString(file);

		ShaderProgramInclude includer;

		ID3D10Blob *errorMessage = nullptr;
		HRESULT hr = D3DCompileFromFile(wfile.c_str(), nullptr, &includer, entrypoint, target, D3D10_SHADER_ENABLE_STRICTNESS, 0, outBuffer, &errorMessage);
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



	bool NGAShader::Construct(const char *filename, NGAShaderType type)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);

		const bool compiled = CompileShader(&mBytecode, filename, type);
		CORE_ASSERT_RETURN_VALUE(compiled, false, "Failed to compile shader %s with target %s", filename, COMPILE_TARGETS[(int)type]);

		mType = type;

		HRESULT hr;
		switch (mType)
		{
		case NGAShaderType::VERTEX:
			hr = NgaDx11State.mDevice->CreateVertexShader(mBytecode->GetBufferPointer(), mBytecode->GetBufferSize(), nullptr, &mVertexShader);
			break;

		case NGAShaderType::PIXEL:
			hr = NgaDx11State.mDevice->CreatePixelShader(mBytecode->GetBufferPointer(), mBytecode->GetBufferSize(), nullptr, &mPixelShader);
			break;

		default:
			CORE_ASSERT_RETURN_VALUE(false, false, "Unknown shader type (%d)", (int)type);
		}

		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create shader %s", filename);

		return true;
	}

	void NGAShader::Destruct()
	{
		switch (mType)
		{
		case NGAShaderType::VERTEX:
			COM_SAFE_RELEASE(mVertexShader);
			break;

		case NGAShaderType::PIXEL:
			COM_SAFE_RELEASE(mPixelShader);
			break;
		}
	}

	bool NGAShader::IsConstructed()const
	{
		return mVertexShader != nullptr;
	}

	void* NGAShader::GetBytecode()const
	{
		return mBytecode->GetBufferPointer();
	}
	
	size_t NGAShader::GetBytecodeSize()const
	{
		return mBytecode->GetBufferSize();
	}

	NGAShaderType NGAShader::GetType()const
	{
		return mType;
	}
}

#endif