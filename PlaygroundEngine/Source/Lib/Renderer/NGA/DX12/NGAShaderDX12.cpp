#include "NGA/NGAShader.h"

#if CORE_RENDER_API(DX12)

#include <string>

#include <d3dcompiler.h>

#include "Core/File/File.h"
#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"

#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAShader);


	bool NGAShader::Construct(const char* filename, NGAShaderType type)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGAShader::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGAShader::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void* NGAShader::GetBytecode()const
	{
		CORE_UNIMPLEMENTED();
		return nullptr;
	}

	size_t NGAShader::GetBytecodeSize()const
	{
		CORE_UNIMPLEMENTED();
		return 0;
	}

	NGAShaderType NGAShader::GetType()const
	{
		CORE_UNIMPLEMENTED();
		return NGAShaderType::VERTEX;
	}
}

#endif