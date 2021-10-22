#include "NGA/NGACommandContext.h"

#if CORE_RENDER_API(DX12)

#include "NGA/NGAInputLayout.h"
#include "NGA/NGAResources.h"
#include "NGA/NGAResourceViews.h"
#include "NGA/NGASamplerState.h"
#include "NGA/NGAPipelineState.h"
#include "NGACoreInternalDX12.h"

namespace playground
{
	void NGACommandContext::BindPipelineState(const NGAPipelineState& pipelineState)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::DrawIndexed(unsigned int indexCount)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::MapBufferData(const NGABuffer& buffer, const void* data)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::SetViewport(const NGARect& rect, float minDepth, float maxDepth)
	{
		D3D12_VIEWPORT vp;
		vp.TopLeftX = rect.x;
		vp.TopLeftY = rect.y;
		vp.Width = rect.width;
		vp.Height = rect.height;
		vp.MinDepth = minDepth;
		vp.MaxDepth = maxDepth;

		NgaDx12State.mCommandList->RSSetViewports(1, &vp);
	}

	void NGACommandContext::SetPrimitiveTopology(NGAPrimitiveTopology primTopology)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindIndexBuffer(const NGABuffer& indexBuffer, NGAIndexBufferType indexBufferType)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindVertexBuffer(const NGABuffer& vertexBuffer, size_t vertexStride)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindInputLayout(const NGAInputLayout& inputLayout)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindShader(const NGAShader& shader)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindConstantBuffer(const NGABuffer& constantBuffer, NGAShaderStage stage, int slot)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindShaderResource(const NGAShaderResourceView& view, NGAShaderStage stage, int slot)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindSamplerState(const NGASamplerState& samplerState, NGAShaderStage stage, int slot)
	{
		CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::ClearRenderTarget(const NGARenderTargetView& renderTargetView, const float* clearColor)
	{
		NgaDx12State.mCommandList->ClearRenderTargetView(renderTargetView.mDescriptorHandle, clearColor, 0, nullptr);
	}

	void NGACommandContext::ClearDepthStencilView(const NGADepthStencilView& depthStencilView)
	{
		NgaDx12State.mCommandList->ClearDepthStencilView(depthStencilView.mDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0.0f, 0, nullptr);
	}

	void NGACommandContext::BindRenderTarget(const NGARenderTargetView& renderTarget, const NGADepthStencilView& depthStencilView)
	{
		const int numRenderTargets = renderTarget.IsConstructed() ? 1 : 0;
		NgaDx12State.mCommandList->OMSetRenderTargets(numRenderTargets, &renderTarget.mDescriptorHandle, true, &depthStencilView.mDescriptorHandle);
	}
}

#endif // CORE_RENDER_API(DX11)