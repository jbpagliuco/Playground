#include "ForwardSceneRenderer.h"

#include "Core/File/File.h"
#include "Core/Util/Color.h"

#include "Renderer/Pipelines/DebugTexture.h"
#include "Renderer/Pipelines/SkyboxPipeline.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Scene/Camera.h"
#include "Renderer/Scene/Renderables/RenderableInstance.h"
#include "Renderer/Scene/Scene.h"
#include "Renderer/Shader/Shader.h"

namespace playground
{
	bool ForwardRenderer::Initialize()
	{
#if RENDER_FEATURE(SHADOWS)
		bool success = mShadowMapBuilder.Initialize(MAX_SHADOWMAPS);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize shadowmap builder.");

		// Register engine assets
		RegisterEngineRenderTarget("shadowMap", &mShadowMapBuilder.GetRenderTarget());
#endif

		return true;
	}

	void ForwardRenderer::Shutdown()
	{
#if RENDER_FEATURE(SHADOWS)
		mShadowMapBuilder.Shutdown();
#endif
	}

	void ForwardRenderer::BeginRender(Scene& scene)
	{
		MapPerFrameData(scene);

		// Open the command list.
		Playground_RendererStateManager->OpenCommandList();

		// Set the viewport.
		NGARect r;
		r.x = 0.0f;
		r.y = 0.0f;
		r.width = (float)Playground_Renderer->GetWindow().width;
		r.height = (float)Playground_Renderer->GetWindow().height;
		Playground_RendererStateManager->SetViewport(r);
	}

	void ForwardRenderer::EndRender(Scene& scene)
	{
		// Present the main render target
		Playground_MainRenderTarget->Present(Playground_SwapChain->GetBufferIndex());

		// Close and flush the command list.
		Playground_RendererStateManager->CloseCommandList();
	}

	void ForwardRenderer::RenderScene(Scene &scene, const Camera &camera)
	{
#if RENDER_FEATURE(SHADOWS)
		BuildShadowMaps(scene, camera);
#endif

		RenderSceneToBackBuffer(scene, camera);

		// Unbind shadow maps
		Playground_RendererStateManager->BindShaderResource(NGAShaderResourceView::INVALID, NGA_SHADER_STAGE_PIXEL, (int)TextureRegisters::SHADOWMAP);
		Playground_RendererStateManager->BindSamplerState(NGASamplerState::INVALID, NGA_SHADER_STAGE_PIXEL, (int)SamplerStateRegisters::SHADOWMAP);
	}



	void ForwardRenderer::MapPerFrameData(Scene& scene)
	{
		PerFrameData perFrameData;

		// Collect shadow casting lights from the scene.
#if RENDER_FEATURE(SHADOWS)
		CollectShadowCastingLights(scene);
		for (int i = 0; i < MAX_SHADOWMAPS; ++i) {
			if (mShadowCastingLights[i] != nullptr) {
				perFrameData.lightViewProj[i] = mShadowCastingLights[i]->GetViewProj();
			}
		}
#endif

		// Map per frame data.
		// TODO: Doesn't work with multiple cameras.
		auto& lights = scene.GetLights();
		for (const auto& camera : scene.GetCameras()) {
			constexpr float ambient = 0.3f;
			const Vector3f cameraPos = camera->mTransform.mPosition.AsVector3();

			LightsData lightsData;
			lightsData.globalAmbient = Tuple4f(ambient, ambient, ambient, 1.0f);
			lightsData.eyePosition = Tuple3f(cameraPos.x, cameraPos.y, cameraPos.z);
			lightsData.numLights = (int)lights.size();
			for (int i = 0; i < lightsData.numLights; ++i) {
				lightsData.lights[i] = *lights[i];
			}

			perFrameData.cameraViewProj = camera->GetViewProj();
			perFrameData.mNumShadowCasters = mNumShadowCastingLights;

			Playground_RendererStateManager->MapPerFrameData(perFrameData);
			Playground_RendererStateManager->MapLightsData(lightsData);
		}

		// Map per object data for all objects
		PerObjectData perObjectData[MAX_RENDER_OBJECTS];
		for (const auto& it : scene.GetRenderables()) {
			const auto& bucket = it.second;
			for (const auto& renderable : bucket) {
				perObjectData[renderable.mObjectIndex].mWorld = renderable.mRenderable->GetWorldTransform();
				perObjectData[renderable.mObjectIndex].mWorldInverseTranspose = perObjectData[renderable.mObjectIndex].mWorld.Inverted().Transposed();
			}
		}
		Playground_RendererStateManager->MapPerObjectData(perObjectData, MAX_RENDER_OBJECTS);
	}

	void ForwardRenderer::CollectShadowCastingLights(Scene& scene)
	{
#if RENDER_FEATURE(SHADOWS)
		// Clear out our shadow casting light list
		for (int i = 0; i < MAX_SHADOWMAPS; ++i) {
			mShadowCastingLights[i] = nullptr;
		}

		int shadowCasterIndex = 0;
		for (auto& light : scene.GetLights()) {
			// Invalidate shadow casting index
			light->mShadowCastingIndex = -1;

			// Add this light to the list of shadow casters, if possible.
			if (shadowCasterIndex < MAX_SHADOWMAPS) {
				// Is this a shadow casting light?
				if (light->mLightType == LightType::DIRECTIONAL && light->mEnabled) {
					mShadowCastingLights[shadowCasterIndex] = light;
					mShadowCastingLights[shadowCasterIndex]->mShadowCastingIndex = shadowCasterIndex;

					++shadowCasterIndex;
				}
			}
		}

		mNumShadowCastingLights = shadowCasterIndex;
#endif
	}

	void ForwardRenderer::BuildShadowMaps(Scene &scene, const Camera &camera)
	{
#if RENDER_FEATURE(SHADOWS)
		mShadowMapBuilder.BuildAll(scene, (const Light**)mShadowCastingLights, mNumShadowCastingLights);
#endif
	}
	
	void ForwardRenderer::RenderSceneToBackBuffer(Scene &scene, const Camera &camera)
	{
		Playground_RendererStateManager->ClearAllUserResources();

		// Bind render target
		RenderTarget* rt = (camera.mRenderTarget == nullptr) ? Playground_MainRenderTarget : camera.mRenderTarget;
		rt->Bind(Playground_SwapChain->GetBufferIndex());
		rt->Clear(ColorF(COLOR_CORNFLOWERBLUE).FloatArray(), true, Playground_SwapChain->GetBufferIndex());

		// Bind shadow map textures
#if RENDER_FEATURE(SHADOWS)
		const Texture &depthMap = mShadowMapBuilder.GetRenderTarget().GetDepthMap();
		Playground_RendererStateManager->BindShaderResource(depthMap.GetShaderResourceView(), NGA_SHADER_STAGE_PIXEL, (int)TextureRegisters::SHADOWMAP);
		Playground_RendererStateManager->BindSamplerState(depthMap.GetSamplerState(), NGA_SHADER_STAGE_PIXEL, (int)SamplerStateRegisters::SHADOWMAP);
#endif

		// Render all the renderables in the scene
		for (auto &it : scene.GetRenderables()) {
			const NGAPipelineState* pso = it.first;
			const RenderableBucket bucket = it.second;

			Playground_RendererStateManager->BindPipelineState(*pso);

			Playground_RendererStateManager->BindPerFrameData();
			Playground_RendererStateManager->BindLightsData();

			for (auto& renderable : bucket) {
				Playground_RendererStateManager->BindPerObjectData(renderable.mObjectIndex);
				renderable.mRenderable->Render();
			}
		}

		// Draw skybox last
		if (camera.mSkybox != nullptr) {
			 RenderSkybox(*camera.mSkybox, camera);
		}

		// Draw debug texture, if set
		RenderDebugTexture();
	}
}