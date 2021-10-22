#include "NGACoreInternalDX11.h"

#if CORE_RENDER_API(DX11)

#include <map>

#include "Core/Util/Util.h"

namespace playground
{
	NGAInternalStateDX11 NgaDx11State;

	D3D11_USAGE NGAUsageToD3D11(NGAUsage usage)
	{
		static constexpr D3D11_USAGE CONV[] = {
			D3D11_USAGE_IMMUTABLE,
			D3D11_USAGE_DEFAULT,
			D3D11_USAGE_DYNAMIC,
			D3D11_USAGE_STAGING
		};

		return CONV[(int)usage];
	}
}

#endif // CORE_RENDER_API(DX11)