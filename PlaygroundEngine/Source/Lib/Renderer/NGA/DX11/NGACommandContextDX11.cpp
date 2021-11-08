#include "NGA/NGACommandContext.h"

#if CORE_RENDER_API(DX11)

#include "NGA/NGAInputLayout.h"
#include "NGA/NGAResources.h"
#include "NGA/NGAResourceViews.h"
#include "NGA/NGASamplerState.h"
#include "NGA/NGAPipelineState.h"
#include "NGACoreInternalDX11.h"

namespace playground
{
	void NGACommandContext::BindPipelineState(const NGAPipelineState &pipelineState)
	{
		NgaDx11State.mContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)pipelineState.mPrimitiveTopology);

		NgaDx11State.mContext->RSSetState(pipelineState.mRasterizerState);
		NgaDx11State.mContext->OMSetDepthStencilState(pipelineState.mDepthStencilState, 0);
	}

	void NGACommandContext::DrawIndexed(unsigned int indexCount)
	{
		NgaDx11State.mContext->DrawIndexed(indexCount, 0, 0);
	}

	void NGACommandContext::MapBufferData(const NGABuffer &buffer, const void *data)
	{
		const NGABufferUsage usage = buffer.mDesc.mUsage;
		CORE_ASSERT_RETURN((usage & NGA_BUFFER_USAGE_CPU_WRITE) || (usage & NGA_BUFFER_USAGE_CPU_READ_WRITE));

		D3D11_MAPPED_SUBRESOURCE res;

		HRESULT hr = NgaDx11State.mContext->Map(buffer.mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
		CORE_ASSERT_RETURN(SUCCEEDED(hr), "ID3D11DeviceContext::Map() failed with HRESULT %X", hr);

		memcpy(res.pData, data, buffer.mDesc.mSizeInBytes);

		NgaDx11State.mContext->Unmap(buffer.mBuffer, 0);
	}

	void NGACommandContext::SetViewport(const NGARect &rect, float minDepth, float maxDepth)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = rect.x;
		vp.TopLeftY = rect.y;
		vp.Width = rect.width;
		vp.Height = rect.height;
		vp.MinDepth = minDepth;
		vp.MaxDepth = maxDepth;

		NgaDx11State.mContext->RSSetViewports(1, &vp);
	}

	void NGACommandContext::SetPrimitiveTopology(NGAPrimitiveTopology primTopology)
	{
		NgaDx11State.mContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)primTopology);
	}

	void NGACommandContext::BindIndexBuffer(const NGABuffer &indexBuffer)
	{
		NgaDx11State.mContext->IASetIndexBuffer(indexBuffer.mBuffer, NGAFormatToDXGI(indexBuffer.GetIndexBufferFormat()), 0);
	}

	void NGACommandContext::BindVertexBuffer(const NGABuffer &vertexBuffer)
	{
		UINT stride = (UINT)vertexBuffer.GetVertexStride();
		UINT offset = 0;
		NgaDx11State.mContext->IASetVertexBuffers(0, 1, &vertexBuffer.mBuffer, &stride, &offset);
	}

	void NGACommandContext::BindInputLayout(const NGAInputLayout &inputLayout)
	{
		NgaDx11State.mContext->IASetInputLayout(inputLayout.mInputLayout);
	}

	void NGACommandContext::BindConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot)
	{
		if (stage & NGA_SHADER_STAGE_VERTEX) {
			NgaDx11State.mContext->VSSetConstantBuffers(slot, 1, &constantBuffer.mBuffer);
		}

		if (stage & NGA_SHADER_STAGE_PIXEL) {
			NgaDx11State.mContext->PSSetConstantBuffers(slot, 1, &constantBuffer.mBuffer);
		}
	}

	void NGACommandContext::BindShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot)
	{
		if (stage & NGA_SHADER_STAGE_VERTEX) {
			NgaDx11State.mContext->VSSetShaderResources(slot, 1, &view.mView);
		}

		if (stage & NGA_SHADER_STAGE_PIXEL) {
			NgaDx11State.mContext->PSSetShaderResources(slot, 1, &view.mView);
		}
	}

	void NGACommandContext::BindSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot)
	{
		if (stage & NGA_SHADER_STAGE_VERTEX) {
			NgaDx11State.mContext->VSSetSamplers(slot, 1, &samplerState.mSamplerState);
		}

		if (stage & NGA_SHADER_STAGE_PIXEL) {
			NgaDx11State.mContext->PSSetSamplers(slot, 1, &samplerState.mSamplerState);
		}
	}

	void NGACommandContext::ClearRenderTarget(const NGARenderTargetView &renderTargetView, const float *clearColor)
	{
		NgaDx11State.mContext->ClearRenderTargetView(renderTargetView.mView, clearColor);
	}

	void NGACommandContext::ClearDepthStencilView(const NGADepthStencilView &depthStencilView)
	{
		NgaDx11State.mContext->ClearDepthStencilView(depthStencilView.mView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void NGACommandContext::BindRenderTarget(const NGARenderTargetView &renderTarget, const NGADepthStencilView &depthStencilView)
	{
		const int numRenderTargets = renderTarget.IsConstructed() ? 1 : 0;
		NgaDx11State.mContext->OMSetRenderTargets(numRenderTargets, &renderTarget.mView, depthStencilView.mView);
	}
}

#endif // CORE_RENDER_API(DX11)