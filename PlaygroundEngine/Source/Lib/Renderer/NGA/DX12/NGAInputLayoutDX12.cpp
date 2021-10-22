#include "NGA/NGAInputLayout.h"

#if CORE_RENDER_API(DX12)

#include "NGA/NGAShader.h"
#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAInputLayout);

	bool NGAInputLayout::Construct(const NGAVertexFormatDesc& vertexFormatDesc, const NGAShader& vertexShader)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGAInputLayout::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGAInputLayout::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}
}

#endif