#include "NGACoreInternalDX12.h"

#if CORE_RENDER_API(DX12)

#include <map>

#include "Core/Util/Util.h"

namespace playground
{
	NGAInternalStateDX12 NgaDx12State;


	void SetDebugName(IDXGIObject* object, const char* name)
	{
		object->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
	}
	
	void SetDebugName(ID3D12Object* object, const char* name)
	{
		object->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
	}
}

#endif // CORE_RENDER_API(DX12)