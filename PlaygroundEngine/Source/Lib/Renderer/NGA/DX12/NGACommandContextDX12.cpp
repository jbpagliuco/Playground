#include "NGA/NGACommandContext.h"

#if CORE_RENDER_API(DX12)

#include "Core/Debug/Log.h"
#include "Core/Util/Timer.h"

#include "NGA/NGAResources.h"
#include "NGA/NGAResourceViews.h"
#include "NGA/NGASamplerState.h"
#include "NGA/NGAPipelineState.h"
#include "NGACoreInternalDX12.h"

namespace playground
{
	void NGACommandContext::Reset()
	{
		HRESULT hr = NgaDx12State.mCommandAllocator->Reset();
		CORE_ASSERT(SUCCEEDED(hr), "Failed to reset command allocator.");

		hr = NgaDx12State.mCommandList->Reset(NgaDx12State.mCommandAllocator, nullptr);
		CORE_ASSERT(SUCCEEDED(hr), "Failed to reset command list.");
	}

	void NGACommandContext::Close()
	{
		NgaDx12State.mCommandList->Close();

		ID3D12CommandList* cmdLists[] = { NgaDx12State.mCommandList };
		NgaDx12State.mCommandQueue->ExecuteCommandLists(1, cmdLists);
	}

	void NGACommandContext::Flush()
	{
		// Increment the fence value.
		++NgaDx12State.mCurrentFence;

		// Process all commands prior to this one.
		NgaDx12State.mCommandQueue->Signal(NgaDx12State.mFence, NgaDx12State.mCurrentFence);

		// Wait for the GPU to complete all commands.
		if (NgaDx12State.mFence->GetCompletedValue() < NgaDx12State.mCurrentFence) {
			// Fire event when the GPU hits the fence.
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			NgaDx12State.mFence->SetEventOnCompletion(NgaDx12State.mCurrentFence, eventHandle);

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}



	void NGACommandContext::BindPipelineState(const NGAPipelineState& pipelineState)
	{
		NgaDx12State.mCommandList->SetPipelineState(pipelineState.mPSO);
		NgaDx12State.mCommandList->SetGraphicsRootSignature(pipelineState.mRootSignature);
	}

	void NGACommandContext::DrawIndexed(unsigned int indexCount)
	{
		NgaDx12State.mCommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}

	void NGACommandContext::MapBufferData(const NGABuffer& buffer, const void* data, size_t size, int arrayIndex)
	{
		const NGABufferUsage usage = buffer.mDesc.mUsage;
		CORE_ASSERT_RETURN((usage & NGA_BUFFER_USAGE_CPU_WRITE) || (usage & NGA_BUFFER_USAGE_CPU_READ_WRITE));

		CORE_ASSERT_RETURN(arrayIndex >= 0 && arrayIndex < buffer.mDesc.mArraySize, "Trying to write to an invalid array index %d", arrayIndex);

		BYTE* mappedData;
		HRESULT hr = buffer.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		CORE_ASSERT_RETURN(SUCCEEDED(hr), "Failed to map constant buffer data.");

		memcpy(mappedData + (buffer.GetBufferElementSize() * arrayIndex), data, size);

		buffer.mBuffer->Unmap(0, nullptr);
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

		D3D12_RECT scissorRect;
		scissorRect.left = rect.x;
		scissorRect.top = rect.y;
		scissorRect.right = rect.x + rect.width;
		scissorRect.bottom = rect.y + rect.height;

		NgaDx12State.mCommandList->RSSetViewports(1, &vp);
		NgaDx12State.mCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void NGACommandContext::SetPrimitiveTopology(NGAPrimitiveTopology primTopology)
	{
		NgaDx12State.mCommandList->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)primTopology);
	}

	void NGACommandContext::BindIndexBuffer(const NGABuffer& indexBuffer)
	{
		NgaDx12State.mCommandList->IASetIndexBuffer(&indexBuffer.mIndexBufferView);
	}

	void NGACommandContext::BindVertexBuffer(const NGABuffer& vertexBuffer)
	{
		NgaDx12State.mCommandList->IASetVertexBuffers(0, 1, &vertexBuffer.mVertexBufferView);
	}

	void NGACommandContext::BindShader(const NGAShader& shader)
	{
		// CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindConstantBuffer(const NGABuffer& constantBuffer, NGAShaderStage stage, int slot, int arrayIndex)
	{
		NgaDx12State.mCommandList->SetGraphicsRootConstantBufferView(slot, constantBuffer.GetGPUVirtualAddress(arrayIndex));
	}

	void NGACommandContext::BindShaderResource(const NGAShaderResourceView& view, NGAShaderStage stage, int slot)
	{
		// CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::BindSamplerState(const NGASamplerState& samplerState, NGAShaderStage stage, int slot)
	{
		// CORE_UNIMPLEMENTED();
	}

	void NGACommandContext::ClearRenderTarget(const NGARenderTargetView& renderTargetView, const float* clearColor)
	{
		const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetView.mResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);

		NgaDx12State.mCommandList->ClearRenderTargetView(renderTargetView.mDescriptorHandle, clearColor, 0, nullptr);
	}

	void NGACommandContext::ClearDepthStencilView(const NGADepthStencilView& depthStencilView)
	{
		NgaDx12State.mCommandList->ClearDepthStencilView(depthStencilView.mDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0.0f, 0, nullptr);
	}

	void NGACommandContext::BindRenderTarget(const NGARenderTargetView& renderTargetView, const NGADepthStencilView& depthStencilView)
	{
		const int numRenderTargets = renderTargetView.IsConstructed() ? 1 : 0;
		NgaDx12State.mCommandList->OMSetRenderTargets(numRenderTargets, &renderTargetView.mDescriptorHandle, true, &depthStencilView.mDescriptorHandle);
	}

	void NGACommandContext::PresentRenderTarget(const NGARenderTargetView& renderTargetView)
	{
		const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetView.mResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);
	}

	
	void NGACommandContext::UpdateResource(const NGABuffer& destBuffer, const NGABuffer& uploadBuffer, void* data)
	{
		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = uploadBuffer.GetBufferSize();
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource. At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap. Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = destBuffer.mBuffer;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);

		UpdateSubresources<1>(NgaDx12State.mCommandList, destBuffer.mBuffer, uploadBuffer.mBuffer, 0, 0, 1, &subResourceData);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);
	}
}

#endif // CORE_RENDER_API(DX11)