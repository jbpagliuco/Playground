#include "NGA/NGASamplerState.h"

#if CORE_RENDER_API(DX12)

#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGASamplerState);

	bool NGASamplerState::Construct(const NGASamplerStateDesc& desc)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGASamplerState::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGASamplerState::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}
}

#endif