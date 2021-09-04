#pragma once

#define BASE_LOG_FILTER "Base"

namespace playground
{
	bool BaseSystemInit();
	void BaseSystemShutdown();
	void BaseSystemDoFrame(float DeltaTime);

	void BaseSystemDebugRender();
}