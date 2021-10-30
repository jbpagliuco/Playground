#include "NGA/NGAShader.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAShader);

	NGAShaderType NGAShader::GetType()const
	{
		return mType;
	}
}