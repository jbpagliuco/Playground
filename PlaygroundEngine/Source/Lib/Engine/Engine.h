#pragma once

#include <stdint.h>

#define ENGINE_LOG_FILTER "Engine"

namespace playground
{
	bool InitializeEngine();
	void ShutdownEngine();

	void DoFrame();

	float GetEngineDeltaFrameTime();
	double GetEngineElapsedTime();

	uint64_t GetEngineElapsedTimeMilliseconds();
}