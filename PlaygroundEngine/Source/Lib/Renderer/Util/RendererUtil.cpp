#include "RendererUtil.h"

#include "Renderer/Shader/Shader.h"

namespace playground
{
	PipelineStateDesc::PipelineStateDesc(const Shader& shader) :
		PipelineStateDesc()
	{
		mVertexFormat = shader.GetVertexFormat();
		mVertexShader = &shader.GetVertexShader().GetShader();
		mPixelShader = &shader.GetPixelShader().GetShader();
	}
}