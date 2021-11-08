#include "NGA/NGAShader.h"

#if CORE_RENDER_API(DX12)

#include <string>

#include <d3dcompiler.h>

#include "Core/File/File.h"
#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"

#include "NGACoreInternalDX12.h"
#include "NGA/Common/NGADirectXUtil.h"

namespace playground
{
	bool NGAShader::Construct(const char* filename, NGAShaderType type)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		
		mType = type;

		const bool compiled = CompileHLSL(mBytecode, filename, type);
		if (!compiled) {
			CORE_ASSERT("Failed to compile shader %s", filename);
			Destruct();
			return false;
		}

		return true;
	}

	void NGAShader::Destruct()
	{
		mBytecode.Destruct();
	}

	bool NGAShader::IsConstructed()const
	{
		return mBytecode.IsConstructed();
	}
	
	void* NGAShader::GetBytecode()const
	{
		return mBytecode.GetBufferPointer();
	}

	size_t NGAShader::GetBytecodeSize()const
	{
		return mBytecode.GetBufferSize();
	}

	NGAShader::operator D3D12_SHADER_BYTECODE()const
	{
		D3D12_SHADER_BYTECODE bytecode;
		bytecode.pShaderBytecode = GetBytecode();
		bytecode.BytecodeLength = GetBytecodeSize();

		return bytecode;
	}
}

#endif