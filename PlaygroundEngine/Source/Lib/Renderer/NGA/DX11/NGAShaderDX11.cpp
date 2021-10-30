#include "NGA/NGAShader.h"

#if CORE_RENDER_API(DX11)

#include <string>

#include "Core/File/File.h"
#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"
#include "NGACoreInternalDX11.h"

namespace playground
{
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