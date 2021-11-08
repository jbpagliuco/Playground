#include "DebugTexture.h"

#include "Renderer/Mesh.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Renderer.h"
#include "Renderer/Resources/ConstantBuffer.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Util/RendererUtil.h"

namespace playground
{
	static ConstantBuffer DebugTextureCB;
	static const NGAPipelineState* DebugTexturePSO = nullptr;

	bool DebugTextureSystemInitialize()
	{
		bool success = DebugTextureCB.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(Matrix));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize debug texture constant buffer.");

		PipelineStateDesc psoDesc(*GetEngineShader(EngineShader::DEBUG_TEXTURE));
		DebugTexturePSO = Playground_Renderer->FindOrCreatePSO(psoDesc, "Debug Texture");

		return true;
	}

	void DebugTextureSystemShutdown()
	{
		DebugTextureCB.Shutdown();
	}

	void RenderDebugTexture()
	{
		const Texture* debugTex = TryGetEngineTexture("debug");
		if (debugTex != nullptr) {
			// Bind shader
			Playground_RendererStateManager->BindPipelineState(*DebugTexturePSO);

			// Scale and shift quad to lower-right corner.
			Matrix world = Matrix::Translation(Vector(0.5f, -0.5f, 0.0f, 1.0f)) * Matrix::Scaling(Vector(0.5f, 0.5f, 1.0f, 1.0f));
			DebugTextureCB.Map(&world);
			Playground_RendererStateManager->BindConstantBuffer(DebugTextureCB.GetBuffer(), NGA_SHADER_STAGE_VERTEX, 0);

			// Bind the texture
			Playground_RendererStateManager->BindShaderResource(debugTex->GetShaderResourceView(), NGA_SHADER_STAGE_PIXEL, 0);
			Playground_RendererStateManager->BindSamplerState(debugTex->GetSamplerState(), NGA_SHADER_STAGE_PIXEL, 0);

			// Render the quad
			GetEngineMesh(EngineMesh::QUAD)->Render();
		}
	}
}