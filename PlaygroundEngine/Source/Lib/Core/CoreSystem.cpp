#include "CoreSystem.h"

#include "Console/Console.h"
#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "OS/OS.h"
#include "Memory/Memory.h"
#include "Streaming/Stream.h"

namespace playground
{
	bool BaseSystemInit()
	{
#define SYSTEM_INIT(name, f) LogInfo(BASE_LOG_FILTER, "Initializing %s", name); if (!f()) { CORE_ASSERT_RETURN_VALUE(false, false, "Failed to initialize %s", name); }

		SYSTEM_INIT("Streaming system", StreamSystemInit);

		return true;

#undef SYSTEM_INIT
	}

	void BaseSystemShutdown()
	{
#define SYSTEM_SHUTDOWN(name, f) LogInfo(BASE_LOG_FILTER, "Shutting down %s", name); f()
		SYSTEM_SHUTDOWN("Streaming system", StreamSystemShutdown);
		SYSTEM_SHUTDOWN("Memory system", MemorySystemShutdown);
#undef SYSTEM_SHUTDOWN
	}

	void BaseSystemDoFrame(float DeltaTime)
	{
	}

	void BaseSystemDebugRender()
	{
		ConsoleSystemDebugRender();
		StreamSystemDebugRender();
	}
}