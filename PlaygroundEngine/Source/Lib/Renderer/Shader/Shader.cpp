#include "Shader.h"

#include "Renderer/Renderer.h"

namespace playground
{
	CORE_FACTORY_SETUP(Shader);

	bool Shader::Initialize(const ShaderDesc& shaderDesc)
	{
		mVertexShader.Initialize(shaderDesc.mFilename, shaderDesc.mVertexFormatDesc);
		mPixelShader.Initialize(shaderDesc.mFilename);

		return true;
	}

	void Shader::Shutdown()
	{
		mVertexShader.Shutdown();
		mPixelShader.Shutdown();
	}

	void Shader::Bind()
	{
		Playground_RendererStateManager->BindShader(mVertexShader);
		Playground_RendererStateManager->BindShader(mPixelShader);
	}
}