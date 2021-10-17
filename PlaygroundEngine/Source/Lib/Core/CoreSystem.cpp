#include "CoreSystem.h"

#include "Console/Console.h"
#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "OS/OS.h"
#include "Memory/Memory.h"
#include "Streaming/Stream.h"

#if CORE_CONFIG(DEBUG)
#include "CoreReflection_Debug.reflgen.h"
#elif CORE_CONFIG(RELEASE)
#include "CoreReflection_Release.reflgen.h"
#else
#error Unhandled configuration.
#endif

namespace playground
{
	bool BaseSystemInit()
	{
		// Initialize reflection before anything else.
		#if CORE_CONFIG(DEBUG)
		CoreReflection_Debug_InitReflection();
		#elif CORE_CONFIG(RELEASE)
		CoreReflection_Release_InitReflection();
		#else
		#error Unhandled configuration.
		#endif

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