#include "NGA/NGAPipelineState.h"

#if CORE_RENDER_API(DX12)

#include "NGA/NGAShader.h"
#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAPipelineState);

	bool NGAPipelineState::Construct(const NGAFixedFunctionStateDesc& fixedFunctionDesc, const NGAGraphicsPipelineInputAssemblyDesc& inputAssemblyDesc)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGAPipelineState::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGAPipelineState::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}
}

#endif