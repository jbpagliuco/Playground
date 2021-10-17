#include <cstdlib>

#include <Windows.h>

#include "Core/Debug/Assert.h"
#include "Core/Debug/Log.h"
#include "Core/Util/Serialize.h"
#include "Engine/Engine.h"
#include "Engine/World/World.h"

#if CORE_CONFIG(DEBUG)
#include "GameReflection_Debug.reflgen.h"
#elif CORE_CONFIG(RELEASE)
#include "GameReflection_Release.reflgen.h"
#else
#error Unhandled configuration.
#endif

#if CORE_BUILD_TYPE(TOOLS)
#include "Tools.h"
#endif

#if !CORE_BUILD_TYPE(TOOLS)

static int GameMain()
{
	// Initialize reflection
	const bool imported = refl::GetSystemRegistry().Import("Source/Game/GameReflection_Debug.refl");
	CORE_ASSERT_RETURN_VALUE(imported, 1, "Failed to import reflection.");

	#if CORE_CONFIG(DEBUG)
	GameReflection_Debug_InitReflection();
	#elif CORE_CONFIG(RELEASE)
	GameReflection_Release_InitReflection();
	#else
	#error Unhandled configuration.
	#endif

	// Load config
	playground::DeserializationParameterMap config = playground::ParseFile("data/config.xml");

	// Initialize engine
	if (playground::InitializeEngine() == false) {
		return EXIT_FAILURE;
	}

	// Make sure all reflected functions are bound.
	refl::GetSystemRegistry().EnsureFunctionsAreBound();
	
	// Load world.
	playground::LoadWorldFromFile(config["world"].AsString());

	// Run
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	while (true) {
		// Handle the windows messages.
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			break;
		}

		playground::DoFrame();
	}

	// Shutdown
	playground::ShutdownEngine();

	// Success!
	return 0;
}

#endif

int main(int argc, char *argv[])
{
#if CORE_BUILD_TYPE(TOOLS)
	return ToolsMain();
#else
	return GameMain();
#endif
}