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
		// Reset the command context
		void Reset();

		// Closes the command queue.
		void Close();

		// Flush all queued commands.
		void Flush();


		// Bind a pipeline state object.
		void BindPipelineState(const NGAPipelineState &pipelineState);

		// Draw an indexed mesh.
		void DrawIndexed(unsigned int indexCount);

		// Map data to a buffer.
		void MapBufferData(const NGABuffer &buffer, const void *data);

		// Set the current viewport.
		void SetViewport(const NGARect &rect, float minDepth = 0.0f, float maxDepth = 1.0f);



		void SetPrimitiveTopology(NGAPrimitiveTopology primTopology);

		void BindIndexBuffer(const NGABuffer &indexBuffer);
		void BindVertexBuffer(const NGABuffer &vertexBuffer);

		void BindInputLayout(const NGAInputLayout &inputLayout);

		void BindShader(const NGAShader &shader);

		void BindConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot);
		void BindShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot);
		void BindSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot);

		void ClearRenderTarget(const NGARenderTargetView& renderTargetView, const float* clearColor);
		void ClearDepthStencilView(const NGADepthStencilView &depthStencilView);
		void BindRenderTarget(const NGARenderTargetView& renderTargetView, const NGADepthStencilView& depthStencilView);
		void PresentRenderTarget(const NGARenderTargetView& renderTargetView);

#if CORE_RENDER_API(DX12)
	private:
		D3D12_RESOURCE_STATES mCurrentState;
#endif
	};
}