#include "ForwardRenderer.h"

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
		bool success = mShadowMapBuilder.Initialize(MAX_SHADOWMAPS);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize shadowmap builder.");

		success = mRenderPipelineState.Construct(NGAFixedFunctionStateDesc(), NGAGraphicsPipelineInputAssemblyDesc());
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to render pipeline state.");

		// Register engine assets
		RegisterEngineRenderTarget("shadowMap", &mShadowMapBuilder.GetRenderTarget());

		return true;
	}

	void ForwardRenderer::Shutdown()
	{
		mShadowMapBuilder.Shutdown();

		mRenderPipelineState.Destruct();
	}

	void ForwardRenderer::BeginRender()
	{
	}

	void ForwardRenderer::EndRender()
	{
	}

	void ForwardRenderer::RenderScene(Scene &scene, const Camera &camera)
	{
		CollectShadowCastingLights(scene);

		NGARect r;
		r.x = 0.0f;
		r.y = 0.0f;
		r.width = (float)Playground_Renderer->GetWindow().width;
		r.height = (float)Playground_Renderer->GetWindow().height;
		Playground_RendererStateManager->SetViewport(r);

		Playground_RendererStateManager->BindPipelineState(mRenderPipelineState);

		BuildShadowMaps(scene, camera);

		RenderSceneToBackBuffer(scene, camera);

		// Unbind shadow maps
		Playground_RendererStateManager->BindShaderResource(NGAShaderResourceView::INVALID, NGA_SHADER_STAGE_PIXEL, (int)TextureRegisters::SHADOWMAP);
		Playground_RendererStateManager->BindSamplerState(NGASamplerState::INVALID, NGA_SHADER_STAGE_PIXEL, (int)SamplerStateRegisters::SHADOWMAP);
	}


	void ForwardRenderer::CollectShadowCastingLights(Scene& scene)
	{
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
	}

	void ForwardRenderer::BuildShadowMaps(Scene &scene, const Camera &camera)
	{
		mShadowMapBuilder.BuildAll(scene, (const Light**)mShadowCastingLights, mNumShadowCastingLights);
	}
	
	void ForwardRenderer::RenderSceneToBackBuffer(Scene &scene, const Camera &camera)
	{
		Playground_RendererStateManager->ClearAllUserResources();

		NGARect r;
		r.x = 0.0f;
		r.y = 0.0f;
		r.width = (float)Playground_Renderer->GetWindow().width;
		r.height = (float)Playground_Renderer->GetWindow().height;
		Playground_RendererStateManager->SetViewport(r);

		Playground_RendererStateManager->BindPipelineState(mRenderPipelineState);

		// Set per frame data
		Matrix shadowCasterMatrices[MAX_SHADOWMAPS];
		for (int i = 0; i < MAX_SHADOWMAPS; ++i) {
			if (mShadowCastingLights[i] != nullptr) {
				shadowCasterMatrices[i] = mShadowCastingLights[i]->GetViewProj();
			}
		}

		Playground_RendererStateManager->SetPerFrameData(camera.GetViewProj(), shadowCasterMatrices, mNumShadowCastingLights);

		// Bind render target
		RenderTarget* rt = (camera.mRenderTarget == nullptr) ? Playground_MainRenderTarget : camera.mRenderTarget;
		rt->Bind();
		rt->Clear(ColorF(COLOR_CORNFLOWERBLUE).FloatArray(), true);

		// Bind shadow map textures
		const Texture &depthMap = mShadowMapBuilder.GetRenderTarget().GetDepthMap();
		Playground_RendererStateManager->BindShaderResource(depthMap.GetShaderResourceView(), NGA_SHADER_STAGE_PIXEL, (int)TextureRegisters::SHADOWMAP);
		Playground_RendererStateManager->BindSamplerState(depthMap.GetSamplerState(), NGA_SHADER_STAGE_PIXEL, (int)SamplerStateRegisters::SHADOWMAP);

		// Set up shader data
		auto &lights = scene.GetLights();

		constexpr float ambient = 0.3f;
		Vector3f cameraPos = camera.mTransform.mPosition.AsVector3();

		LightsData lightsData;
		lightsData.globalAmbient = Tuple4f(ambient, ambient, ambient, 1.0f);
		lightsData.eyePosition = Tuple3f(cameraPos.x, cameraPos.y, cameraPos.z);
		lightsData.numLights = (int)lights.size();
		for (int i = 0; i < lightsData.numLights; ++i) {
			lightsData.lights[i] = *lights[i];
		}
		Playground_RendererStateManager->SetLightsData(lightsData);

		// Render all the renderables in the scene
		for (auto &r : scene.GetRenderables()) {
			Playground_RendererStateManager->SetObjectTransform(r->GetWorldTransform());
			r->Render();
		}

		// Draw skybox last
		if (camera.mSkybox != nullptr) {
			RenderSkybox(*camera.mSkybox, camera);
		}

		// Draw debug texture, if set
		Playground_RendererStateManager->BindPipelineState(mRenderPipelineState);
		RenderDebugTexture();
	}
}