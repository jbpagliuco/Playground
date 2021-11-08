#pragma once

#include "Renderer/NGA/NGAPipelineState.h"

namespace playground
{
	class Shader;

	struct PipelineStateDesc : public NGAPipelineStateDesc
	{
	public:
		PipelineStateDesc() = default;
		PipelineStateDesc(const Shader& shader);
	};
}