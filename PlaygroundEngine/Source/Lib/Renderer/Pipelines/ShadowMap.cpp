#include "ShadowMap.h"

#include "Core/Math/Matrix.h"
#include "Core/Util/Color.h"

#include "Renderer/NGA/NGAShapes.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Resources/ConstantBuffer.h"
#include "Renderer/Scene/Scene.h"
#include "Renderer/Scene/Renderables/RenderableInstance.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderConstants.h"

namespace playground
{
	static NGARect ShadowMapViewport;

	static ConstantBuffer ShadowMapPerFrameBuffer;
	static ConstantBuffer ShadowMapPerObjectBuffer;


	bool ShadowMapBuilder::Initialize(int numShadowMaps)
	{
		mNumShadowMaps = numShadowMaps;

		RenderTargetDesc desc;
		desc.mWidth = SHADOWMAP_SIZE;
		desc.mHeight = SHADOWMAP_SIZE;

		desc.mUseColorMap = false;

		desc.mUseDepthMap = true;
		desc.mDepthMapDesc.mFormat = NGAFormat::R32_TYPELESS;
		desc.mDepthMapDesc.mShaderResource = true;
		desc.mDepthMapDesc.mType = NGATextureType::TEXTURE2D;
		desc.mDepthMapDesc.mUsage = NGAUsage::GPU_WRITE;
		desc.mDepthMapDesc.mArraySize = mNumShadowMaps;
		desc.mDepthMapDesc.mSamplerStateDesc.mAddressU = NGATextureAddressMode::CLAMP;
		desc.mDepthMapDesc.mSamplerStateDesc.mAddressV = NGATextureAddressMode::CLAMP;
		desc.mDepthMapDesc.mSamplerStateDesc.mAddressW = NGATextureAddressMode::CLAMP;
		desc.mDepthMapDesc.mSamplerStateDesc.mFilter = NGATextureFilter::MIN_MAG_MIP_POINT;

		bool success = mRenderTarget.Initialize(desc);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize shadow map render target.");

		return true;
	}
	
	void ShadowMapBuilder::Shutdown()
	{
		mRenderTarget.Shutdown();
	}

	void ShadowMapBuilder::BuildAll(const Scene& scene, const Light** lights, int numLights)
	{
		// Set viewport
		Playground_RendererStateManager->SetViewport(ShadowMapViewport);

		// Set shaders
		Playground_RendererStateManager->BindShader(GetEngineShader(EngineShader::SHADOWMAP)->GetVertexShader());
		Playground_RendererStateManager->BindShader(GetEngineShader(EngineShader::SHADOWMAP)->GetPixelShader());

		for (int i = 0; i < numLights; ++i) {
			BuildSlice(scene, *lights[i], i);
		}
	}

	RenderTarget& ShadowMapBuilder::GetRenderTarget()
	{
		return mRenderTarget;
	}

	void ShadowMapBuilder::BuildSlice(const Scene& scene, const Light& light, int slice)
	{
		// Set per frame data
		Matrix lightVP = light.GetViewProj();
		Playground_RendererStateManager->MapBufferData(ShadowMapPerFrameBuffer.GetBuffer(), &lightVP);
		Playground_RendererStateManager->BindConstantBuffer(ShadowMapPerFrameBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, 0);

		// Setup render target
		mRenderTarget.Bind(slice);
		mRenderTarget.Clear(ColorF(COLOR_WHITE).FloatArray(), true, slice);

		// Render objects from the view point of the light
		for (auto& it : scene.GetRenderables()) {
			const RenderableBucket bucket = it.second;
			
			// TODO: Bind shadow map pipeline

			for (auto& renderable : bucket) {
				Playground_RendererStateManager->BindPerObjectData(renderable.mObjectIndex);
				Playground_RendererStateManager->BindConstantBuffer(ShadowMapPerObjectBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, 1);

				renderable.mRenderable->Render(false);
			}
		}
	}



	bool ShadowMapSystemInitialize()
	{
		ShadowMapViewport.x = 0.0f;
		ShadowMapViewport.y = 0.0f;
		ShadowMapViewport.width = (float)SHADOWMAP_SIZE;
		ShadowMapViewport.height = (float)SHADOWMAP_SIZE;

		bool success = ShadowMapPerFrameBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(Matrix));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create shadow map per frame buffer.");

		success = ShadowMapPerObjectBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(Matrix));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create shadow map per object buffer.");

		return true;
	}
	
	void ShadowMapSystemShutdown()
	{
		ShadowMapPerObjectBuffer.Shutdown();
		ShadowMapPerFrameBuffer.Shutdown();
	}
}