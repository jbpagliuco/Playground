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

		const bool compiled = CompileHLSL(&mBytecode, filename, type);
		CORE_ASSERT_RETURN_VALUE(compiled, false, "Failed to compile shader %s", filename);

		return true;
	}

	void NGAShader::Destruct()
	{
		COM_SAFE_RELEASE(mBytecode);
	}

	bool NGAShader::IsConstructed()const
	{
		return mBytecode != nullptr;
	}
	
	void* NGAShader::GetBytecode()const
	{
		CORE_ASSERT_RETURN_VALUE(mBytecode != nullptr, nullptr, "Tried to get bytecode of uncompiled shader.");

		return mBytecode->GetBufferPointer();
	}

	size_t NGAShader::GetBytecodeSize()const
	{
		CORE_ASSERT_RETURN_VALUE(mBytecode != nullptr, -1, "Tried to get bytecode size of uncompiled shader.");

		return mBytecode->GetBufferSize();
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