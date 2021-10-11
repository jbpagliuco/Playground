#include <cstdlib>

#include <Windows.h>

#include "Core/Debug/Assert.h"
#include "Core/Debug/Log.h"
#include "Core/Util/Serialize.h"
#include "Engine/Engine.h"
#include "Engine/World/World.h"

#include "GameReflection_Debug.reflgen.h"

#if CORE_BUILD_TYPE(TOOLS)
#include "Tools.h"
#endif

#if !CORE_BUILD_TYPE(TOOLS)

static int GameMain()
{
	playground::DeserializationParameterMap config = playground::ParseFile("data/config.xml");

	// Initialize reflection
	const bool imported = refl::GetSystemRegistry().Import("Source/Game/GameReflection_Debug.refl");
	CORE_ASSERT_RETURN_VALUE(imported, 1, "Failed to import reflection.");

	GameReflection_Debug_InitReflection();

	// Initialize engine
	if (playground::InitializeEngine() == false) {
		return EXIT_FAILURE;
	}
	
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