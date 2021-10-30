#include "SkyboxPipeline.h"

#include "Core/File/File.h"
#include "Core/Math/Matrix.h"

#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Resources/Skybox.h"
#include "Renderer/Scene/Camera.h"
#include "Renderer/Shader/Shader.h"

namespace playground
{
	static NGAPipelineState SkyboxPipelineState;
	static ConstantBuffer SkyboxPerFrameBuffer;
	static Mesh* SkyboxMesh;


	bool SkyboxSystemInitialize()
	{
		const Shader* skyboxShader = GetEngineShader(EngineShader::SKYBOX);

		NGAPipelineStateDesc pipelineDesc;
		pipelineDesc.mVertexShader = &skyboxShader->GetVertexShader().GetShader();
		pipelineDesc.mPixelShader = &skyboxShader->GetPixelShader().GetShader();
		pipelineDesc.mVertexFormat = skyboxShader->GetVertexFormat();

		pipelineDesc.mRasterizerState.mCullMode = NGACullMode::CULL_FRONT;
		pipelineDesc.mDepthStencilState.mDepthFunc = NGADepthFunc::LESS_EQUAL;
		
		bool success = SkyboxPipelineState.Construct(pipelineDesc);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create skybox pipeline.");

		success = SkyboxPerFrameBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(Matrix));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create skybox constant buffer.");

		// TODO: Don't request an asset provided by the game here
		AssetID meshID = RequestAsset("cube.meshx");
		SkyboxMesh = Mesh::Get(meshID);
		CORE_ASSERT_RETURN_VALUE(SkyboxMesh != nullptr, false, "Failed to create skybox mesh.");

		return true;
	}

	void SkyboxSystemShutdown()
	{
		ReleaseAsset(SkyboxMesh->GetID());
		SkyboxPerFrameBuffer.Shutdown();
		SkyboxPipelineState.Destruct();
	}

	void RenderSkybox(const Skybox& skybox, const Camera& camera)
	{
		Playground_RendererStateManager->BindPipelineState(SkyboxPipelineState);

		Matrix vp = camera.GetViewProj() * Matrix::Translation(camera.mTransform.mPosition);
		SkyboxPerFrameBuffer.Map(&vp);
		Playground_RendererStateManager->BindConstantBuffer(SkyboxPerFrameBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, 0);

		Playground_RendererStateManager->BindShaderResource(skybox.GetTexture().GetShaderResourceView(), NGA_SHADER_STAGE_PIXEL, 0);
		Playground_RendererStateManager->BindSamplerState(skybox.GetTexture().GetSamplerState(), NGA_SHADER_STAGE_PIXEL, 0);

		SkyboxMesh->Render();
	}
}