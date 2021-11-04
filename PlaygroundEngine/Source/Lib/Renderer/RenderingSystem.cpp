#include "RenderingSystem.h"

#include <algorithm>
#include <map>

#include "Core/Debug/Assert.h"
#include "Core/File/File.h"
#include "Core/OS/OS.h"
#include "Core/Util/Timer.h"

#include "NGA/NGACore.h"

#include "Renderer/ImguiRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Pipelines/DebugTexture.h"
#include "Renderer/Pipelines/ShadowMap.h"
#include "Renderer/Pipelines/SkyboxPipeline.h"
#include "Renderer/Renderer.h"
#include "Renderer/Scene/Camera.h"
#include "Renderer/Scene/Renderers/EmptySceneRenderer.h"
#include "Renderer/Scene/Renderers/ForwardSceneRenderer.h"
#include "Renderer/Scene/Scene.h"
#include "Renderer/Shader/Shader.h"

#if CORE_CONFIG(DEBUG)
#include "RendererReflection_Debug.reflgen.h"
#elif CORE_CONFIG(RELEASE)
#include "RendererReflection_Release.reflgen.h"
#else
#error Unhandled configuration.
#endif

namespace playground
{
	static constexpr int MAX_FRAMERATE = 1000;
	static Timer RenderFrameTimer;

	Scene MainScene;
	// ForwardRenderer MainSceneRenderer;
	EmptySceneRenderer MainSceneRenderer;

	static std::map<std::string, const Texture*> EngineTextures;
	static std::map<std::string, RenderTarget*> EngineRenderTargets;

	static std::map<EngineMesh, Mesh*> EngineMeshes;
	static std::map<EngineShader, Shader*> EngineShaders;



	static void RegisterEngineMesh(EngineMesh name, const std::string& meshFilename)
	{
		CORE_ASSERT_RETURN(EngineMeshes.find(name) == EngineMeshes.end(), "Registering engine mesh (%d) multiple times.", name);

		std::string fullPath;
		bool success = GetFullFilepath(fullPath, meshFilename);
		CORE_ASSERT_RETURN(success, "Failed to find %s", meshFilename);

		AssetID id = RequestAsset(fullPath);
		EngineMeshes[name] = Mesh::Get(id);
	}

	static void UnregisterAllEngineMeshes()
	{
		for (auto& it : EngineMeshes) {
			ReleaseAsset(it.second->GetID());
		}

		EngineMeshes.erase(EngineMeshes.begin(), EngineMeshes.end());
	}


	static void RegisterEngineShader(EngineShader name, const std::string& shaderFilename)
	{
		CORE_ASSERT_RETURN(EngineShaders.find(name) == EngineShaders.end(), "Registering engine shader (%d) multiple times.", name);

		std::string fullPath;
		bool success = GetFullFilepath(fullPath, shaderFilename);
		CORE_ASSERT_RETURN(success, "Failed to find %s", shaderFilename.c_str());

		AssetID id = RequestAsset(fullPath);
		EngineShaders[name] = Shader::Get(id);
	}

	static void UnregisterAllEngineShaders()
	{
		for (auto& it : EngineShaders) {
			ReleaseAsset(it.second->GetID());
		}

		EngineShaders.erase(EngineShaders.begin(), EngineShaders.end());
	}


	bool RenderingSystemInit()
	{
		// Initialize reflection before anything else.
		#if CORE_CONFIG(DEBUG)
		RendererReflection_Debug_InitReflection();
		#elif CORE_CONFIG(RELEASE)
		RendererReflection_Release_InitReflection();
		#else
		#error Unhandled configuration.
		#endif

		RenderFrameTimer.Start((int)((1.0f / MAX_FRAMERATE) * 1000.0f));

		constexpr int width = 1600;
		constexpr int height = 900;

		RendererInitParams p;
		p.mWidth = width;
		p.mHeight = height;
		p.mWindow = CreateAndShowWindow(-1, -1, width, height, L"Playground");

		CORE_FATAL_ERROR(p.mWindow != INVALID_WINDOW, "Failed to create main window.");

		NGAInitParams ngaParams;
		NGAInitialize(ngaParams);

		bool success = Playground_Renderer->Initialize(p);
		CORE_FATAL_ERROR(success, "Failed to initialize renderer.");

		if (!ImguiRendererSystemInit()) {
			// Not really fatal
			CORE_ASSERT(false, "Failed to initialize ImGui renderer.");
		}

		return true;
	}

	bool RenderingSystemInitLate()
	{
		Playground_RendererStateManager->OpenCommandList();

		// Create engine meshes
		RegisterEngineMesh(EngineMesh::QUAD, "quad.meshx");

		// Create engine shaders
		RegisterEngineShader(EngineShader::DEBUG_TEXTURE, "debug_texture.shaderx");
		RegisterEngineShader(EngineShader::SHADOWMAP, "build_shadow_map.shaderx");
		RegisterEngineShader(EngineShader::SKYBOX, "skybox.shaderx");

		DebugTextureSystemInitialize();
		ShadowMapSystemInitialize();
		SkyboxSystemInitialize();

		MainSceneRenderer.Initialize();

		Playground_RendererStateManager->CloseCommandList();

		return true;
	}

	void RenderingSystemShutdown()
	{
		Playground_RendererStateManager->ResetCommandList();
		Playground_RendererStateManager->CloseCommandList();

		MainSceneRenderer.Shutdown();

		SkyboxSystemShutdown();
		ShadowMapSystemShutdown();
		DebugTextureSystemShutdown();

		UnregisterAllEngineShaders();
		UnregisterAllEngineMeshes();

		ImguiRendererSystemShutdown();

		Playground_Renderer->Shutdown();

		NGAShutdown();
	}

	void RenderingSystemBeginFrame()
	{
	}

	void RenderingSystemDoFrame()
	{
		if (RenderFrameTimer.Elapsed()) {
			Playground_Renderer->BeginRender();
			ImguiRendererBeginFrame();

			MainSceneRenderer.BeginRender();

			Scene* mainScene = Scene::Get();

			const auto& cameras = mainScene->GetCameras();

			// Make sure there's only a single camera rendering to the back buffer.
			size_t numMainCameras = std::count_if(cameras.begin(), cameras.end(), [](const Camera* a) { return a->mEnabled && a->mRenderTarget == nullptr; });
			CORE_ASSERT(numMainCameras == 1, "Only one camera can render to the back buffer.");

			for (const auto& camera : cameras) {
				if (!camera->mEnabled) {
					continue;
				}

				MainSceneRenderer.RenderScene(*mainScene, *camera);
			}

			ImguiRendererEndFrame();
			Playground_Renderer->EndRender();

			MainSceneRenderer.EndRender();

			RenderFrameTimer.Start();
		}
	}

	void RenderingSystemEndFrame()
	{
	}




	void RegisterEngineTexture(const std::string& name, const Texture* texture)
	{
		EngineTextures[name] = texture;
	}

	const Texture* GetEngineTexture(const std::string& name)
	{
		CORE_ASSERT_RETURN_VALUE(EngineTextures.find(name) != EngineTextures.end(), nullptr, "Failed to find engine texture %s", name.c_str());

		return EngineTextures[name];
	}

	const Texture* TryGetEngineTexture(const std::string& name)
	{
		if (EngineTextures.find(name) == EngineTextures.end()) {
			return nullptr;
		}

		return EngineTextures[name];
	}

	void RegisterEngineRenderTarget(const std::string& name, RenderTarget* renderTarget)
	{
		CORE_ASSERT(EngineRenderTargets.find(name) == EngineRenderTargets.end(), "Registering engine render target (%s) multiple times.", name.c_str());

		EngineRenderTargets[name] = renderTarget;
	}
	
	RenderTarget* GetEngineRenderTarget(const std::string& name)
	{
		CORE_ASSERT_RETURN_VALUE(EngineRenderTargets.find(name) != EngineRenderTargets.end(), nullptr, "Failed to find engine render target %s", name.c_str());

		return EngineRenderTargets[name];
	}



	Mesh* GetEngineMesh(EngineMesh mesh)
	{
		CORE_ASSERT_RETURN_VALUE(EngineMeshes.find(mesh) != EngineMeshes.end(), nullptr, "Failed to find engine mesh (%d)", mesh);

		return EngineMeshes[mesh];
	}

	Shader* GetEngineShader(EngineShader shader)
	{
		CORE_ASSERT_RETURN_VALUE(EngineShaders.find(shader) != EngineShaders.end(), nullptr, "Failed to find engine shader (%d)", shader);

		return EngineShaders[shader];
	}
}