#pragma once

#include "Core.h"

#define BASE_LOG_FILTER "Base"

namespace playground
{
	bool BaseSystemInit();
	void BaseSystemShutdown();
	void BaseSystemDoFrame(float DeltaTime);

#if CORE_DEBUG_ENABLE(IMGUI)
	void BaseSystemDebugRender();
#endif
}