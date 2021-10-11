#include "Engine.h"

#include <chrono>
#include <vector>

#include "Core/CoreSystem.h"
#include "Core/Console/Console.h"
#include "Core/Debug/Assert.h"
#include "Core/Debug/DebugImgui.h"
#include "Core/Debug/Log.h"
#include "Core/Util/Timer.h"
#include "Renderer/ImguiRenderer.h"
#include "Renderer/RenderingSystem.h"

#include "Engine/Assets/MeshAsset.h"
#include "Engine/Assets/MaterialAsset.h"
#include "Engine/Assets/TextureAsset.h"
#include "Engine/Components/GameComponent.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/World/World.h"

#include "EngineReflection_Debug.reflgen.h"

namespace playground
{
	struct SystemRegistration
	{
		typedef bool(*InitFunc)();
		typedef void(*Func)();
		typedef void(*DoFrameFunc)(float DeltaTime);

		std::string mSystemName;

		InitFunc mSystemInit;
		InitFunc mSystemInitLate;
		Func mSystemShutdown;
		Func mSystemShutdownLate;

		DoFrameFunc mDoFrame;
		DoFrameFunc mDoFrameLate;

		SystemRegistration() :
			mSystemName(""),
			mSystemInit(nullptr),
			mSystemInitLate(nullptr),
			mSystemShutdown(nullptr),
			mSystemShutdownLate(nullptr),
			mDoFrame(nullptr),
			mDoFrameLate(nullptr)
		{
		}
	};

	static std::vector<SystemRegistration> SystemRegistry;

	// Time stats
	static double FrameTime = 0.0f;
	static uint64_t FrameCount = 0;
	static double ElapsedTime = 0.0f;
	static Timestamp LastFrameEnd;
		
	static Timestamp EngineStartTimestamp;

	constexpr int UPDATE_GAME_TICKS_PER_SECOND = 120;
	constexpr int UPDATE_GAME_SKIP_TICKS = 1000 / UPDATE_GAME_TICKS_PER_SECOND;
	constexpr int UPDATE_GAME_MAX_FRAMESKIP = 5;

	static uint64_t NextGameTickTime;
	static uint64_t LastUpdateGameTickTime;


#if CORE_CONFIG(DEBUG)
	// Debug timing stats
	CONSOLE_BOOL(show_frametime, FrameTimeDebug, false);
	CONSOLE_BOOL(show_frametime_log, FrameTimeDebugLog, true);
#endif

	////////////////////////////////////////////////////////////////

	static void DebugRender();
	static void CheckDebugRendererSwitch();

	static void DebugRenderFrameTime();

	////////////////////////////////////////////////////////////////
	
	static void RegisterSystems()
	{
		{
			SystemRegistration reg;
			reg.mSystemName = "Base System";
			reg.mSystemInit = BaseSystemInit;
			reg.mSystemShutdown = BaseSystemShutdown;
			reg.mDoFrame = BaseSystemDoFrame;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Rendering System";
			reg.mSystemInit = RenderingSystemInit;
			reg.mSystemInitLate = RenderingSystemInitLate;
			reg.mSystemShutdown = RenderingSystemShutdown;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Input System";
			reg.mSystemInit = InputSystemInit;
			reg.mSystemShutdown = InputSystemShutdown;
			reg.mDoFrameLate = InputSystemDoFrameLate;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Physics System";
			reg.mSystemInit = PhysicsSystemInit;
			reg.mSystemShutdown = PhysicsSystemShutdown;
			reg.mDoFrame = PhysicsSystemDoFrame;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Component System";
			reg.mSystemShutdown = GameComponentShutdown;
			reg.mDoFrame = GameComponentDoFrame;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "World System";
			SystemRegistry.push_back(reg);
		}
		
		{
			SystemRegistration reg;
			reg.mSystemName = "Mesh System";
			reg.mSystemInit = MeshSystemInit;
			reg.mSystemShutdownLate = MeshSystemShutdown;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Material System";
			reg.mSystemInit = MaterialSystemInit;
			reg.mSystemShutdownLate = MaterialSystemShutdown;
			SystemRegistry.push_back(reg);
		}

		{
			SystemRegistration reg;
			reg.mSystemName = "Texture System";
			reg.mSystemInit = TextureAssetSystemInit;
			reg.mSystemShutdownLate = TextureAssetSystemShutdown;
			SystemRegistry.push_back(reg);
		}
	}

	bool InitializeEngine()
	{
		// Register reflection data before anything else.
		EngineReflection_Debug_InitReflection();

		RegisterSystems();

		LogInfo(ENGINE_LOG_FILTER, "=============================================");
		for (auto &sys : SystemRegistry)
		{
			LogInfo(ENGINE_LOG_FILTER, "Initializing %s", sys.mSystemName.c_str());
			if (sys.mSystemInit != nullptr) {
				bool ret = sys.mSystemInit();
				CORE_ASSERT_RETURN_VALUE(ret, false, "Failed to initialize system '%s'.", sys.mSystemName.c_str());
			}
		}

		for (auto &sys : SystemRegistry)
		{
			LogInfo(ENGINE_LOG_FILTER, "Initializing %s", sys.mSystemName.c_str());
			if (sys.mSystemInitLate != nullptr) {
				bool ret = sys.mSystemInitLate();
				CORE_ASSERT_RETURN_VALUE(ret, false, "Failed to initialize system '%s'.", sys.mSystemName.c_str());
			}
		}
		LogInfo(ENGINE_LOG_FILTER, "System initialization complete.");
		LogInfo(ENGINE_LOG_FILTER, "=============================================");
		LogLineBreak();

		EngineStartTimestamp = Timestamp::Now();

		LastFrameEnd = Timestamp::Now();

		NextGameTickTime = 0;
		LastUpdateGameTickTime = 0;

		return true;
	}

	void ShutdownEngine()
	{
		// Manually shutdown the world first
		WorldSystemShutdown();

		LogLineBreak();
		LogInfo(ENGINE_LOG_FILTER, "=============================================");
		for (auto it = SystemRegistry.rbegin(); it != SystemRegistry.rend(); ++it) {
			auto &sys = *it;
			LogInfo(ENGINE_LOG_FILTER, "Shutting down %s", sys.mSystemName.c_str());

			if (sys.mSystemShutdown != nullptr) {
				sys.mSystemShutdown();
			}
		}
		LogInfo(ENGINE_LOG_FILTER, "System shutdown complete.");
		LogInfo(ENGINE_LOG_FILTER, "=============================================");
	}

	static void UpdateGame(float deltaTime)
	{
		for (auto& sys : SystemRegistry) {
			if (sys.mDoFrame != nullptr) {
				sys.mDoFrame(deltaTime);
			}
		}

		for (auto& sys : SystemRegistry) {
			if (sys.mDoFrameLate != nullptr) {
				sys.mDoFrameLate(deltaTime);
			}
		}
	}

	static void RenderFrame()
	{
		RenderingSystemBeginFrame();
		RenderingSystemDoFrame();

		DebugRender();

		RenderingSystemEndFrame();
	}

	void DoFrame()
	{
		int numGameTickLoops = 0;
		int elapsedTime = GetEngineElapsedTimeMilliseconds();
		while (elapsedTime > NextGameTickTime && numGameTickLoops < UPDATE_GAME_MAX_FRAMESKIP) {
			// Calculate delta frame time and update game
			const float deltaTime = (elapsedTime - LastUpdateGameTickTime) / 1000.0f;
			UpdateGame(1.0f / UPDATE_GAME_TICKS_PER_SECOND);
			LastUpdateGameTickTime = elapsedTime;

			// Advance
			NextGameTickTime += UPDATE_GAME_SKIP_TICKS;
			++numGameTickLoops;

			elapsedTime = GetEngineElapsedTimeMilliseconds();
		}

		const float interpolation = (float)(GetEngineElapsedTimeMilliseconds() + UPDATE_GAME_SKIP_TICKS - NextGameTickTime) / (float)UPDATE_GAME_SKIP_TICKS;
		RenderFrame();

//#if CORE_CONFIG(DEBUG)
//		if (FrameTimeDebugLog) {
//			const uint64_t newElapsedTime = (uint64_t)(ElapsedTime + FrameTime);
//			if (newElapsedTime - (uint64_t)ElapsedTime >= 1) {
//				LogInfo(ENGINE_LOG_FILTER, "Frame Time: %.5f (%d iterations)", FrameTime, FrameCount);
//			}
//		}
//#endif
//
//		ElapsedTime += FrameTime;
//		++FrameCount;
	}

	float GetEngineDeltaFrameTime()
	{
		return FrameTime;
	}

	double GetEngineElapsedTime()
	{
		return ElapsedTime;
	}

	uint64_t GetEngineElapsedTimeMilliseconds()
	{
		return (Timestamp::Now() - EngineStartTimestamp).AsMilliseconds();
	}
	

	static void DebugRender()
	{
		BaseSystemDebugRender();

		DebugRenderFrameTime();
	}

	static void CheckDebugRendererSwitch()
	{
		// Toggle the debug renderer when F11 is pressed.
		if (IsSystemKeyPressed(VK_F11)) {
			const bool newFocus = !ImguiRendererGetFocus();
			ForceShowCursor(newFocus);
			ImguiRendererSetFocus(newFocus);
		}

		// Toggle the console when F1 is pressed.
		if (IsSystemKeyPressed(VK_F1)) {
			const bool newFocus = !ConsoleSystemIsActive();
			ConsoleSystemActivate(newFocus);
			ForceShowCursor(newFocus);
			ImguiRendererSetFocus(newFocus);
		}
	}


	static void DebugRenderFrameTime()
	{
#if CORE_CONFIG(DEBUG)
		if (FrameTimeDebug) {
			if (ImGui::Begin("Frame Time Debug", &FrameTimeDebug)) {
				ImGui::Text("Frame Time: %.5f (%.1f FPS)", FrameTime, 1.0f / FrameTime);
				ImGui::Text("Total Elapsed Time: %.1f", ElapsedTime);
			}

			ImGui::End();
		}
#endif
	}
}