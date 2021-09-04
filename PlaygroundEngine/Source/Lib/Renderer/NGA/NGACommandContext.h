#pragma once

#include "NGA/NGACommon.h"

#include "NGA/NGAShader.h"
#include "NGA/NGAShapes.h"

namespace playground
{
	class NGABuffer;
	class NGADepthStencilView;
	class NGAInputLayout;
	class NGAPipelineState;
	class NGARasterizerState;
	class NGARenderTargetView;
	class NGASamplerState;
	class NGAShaderResourceView;
	struct NGAViewport;

	class NGACommandContext
	{
	public:
		void BindPipelineState(const NGAPipelineState &pipelineState);

		void DrawIndexed(unsigned int indexCount);

		void MapBufferData(const NGABuffer &buffer, const void *data);

		void SetViewport(const NGARect &rect, float minDepth = 0.0f, float maxDepth = 1.0f);



		void SetPrimitiveTopology(NGAPrimitiveTopology primTopology);

		void BindIndexBuffer(const NGABuffer &indexBuffer, NGAIndexBufferType indexBufferType);
		void BindVertexBuffer(const NGABuffer &vertexBuffer, size_t vertexStride);

		void BindInputLayout(const NGAInputLayout &inputLayout);

		void BindShader(const NGAShader &shader);

		void BindConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot);
		void BindShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot);
		void BindSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot);

		void ClearRenderTarget(const NGARenderTargetView &renderTargetView, const float *clearColor);
		void ClearDepthStencilView(const NGADepthStencilView &depthStencilView);
		void BindRenderTarget(const NGARenderTargetView &renderTargetView, const NGADepthStencilView &depthStencilView);

	};
}