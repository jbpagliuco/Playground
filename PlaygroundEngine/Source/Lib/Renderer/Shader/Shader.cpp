#include "Shader.h"

#include "Renderer/Renderer.h"

namespace playground
{
	CORE_FACTORY_SETUP(Shader);

	bool Shader::Initialize(const ShaderDesc& shaderDesc)
	{
		// Load shader programs
		mVertexShader.Initialize(shaderDesc.mFilename, shaderDesc.mVertexFormatDesc);
		mPixelShader.Initialize(shaderDesc.mFilename);

		NGAPipelineStateDesc psoDesc;
		psoDesc.mVertexShader = &mVertexShader.GetShader();
		psoDesc.mPixelShader = &mPixelShader.GetShader();
		psoDesc.mVertexFormat = shaderDesc.mVertexFormatDesc;

		mVertexFormat = shaderDesc.mVertexFormatDesc;

#if RENDER_DEBUG_FEATURE(STORE_NAMES)
		mDebugName = shaderDesc.mFilename;
#endif

		return true;
	}

	void Shader::Shutdown()
	{
		mVertexShader.Shutdown();
		mPixelShader.Shutdown();
	}
}