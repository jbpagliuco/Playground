#include "NGA/NGAResourceViews.h"

#if CORE_RENDER_API(DX12)

#include "NGACoreInternalDX12.h"

#include "NGA/NGAResources.h"
#include "NGA/NGASwapChain.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAShaderResourceView);
	NGA_GPU_CLASS_IMPLEMENT(NGARenderTargetView);
	NGA_GPU_CLASS_IMPLEMENT(NGADepthStencilView);


	bool NGAShaderResourceView::Construct(const NGATexture& texture)
	{
		CORE_UNIMPLEMENTED();

		return false;
	}

	void NGAShaderResourceView::Destruct()
	{
		// TODO: Free this handle from the heap.
		mDescriptorHandle.Clear();
	}

	bool NGAShaderResourceView::IsConstructed()const
	{
		return mDescriptorHandle.IsValid();
	}

	bool NGAShaderResourceView::operator==(const NGAShaderResourceView& other)const
	{
		return mDescriptorHandle == other.mDescriptorHandle;
	}

	bool NGAShaderResourceView::operator!=(const NGAShaderResourceView& other)const
	{
		return mDescriptorHandle != other.mDescriptorHandle;
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		return this->operator==(view);
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGAShaderResourceView::GenerateMips()const
	{
		CORE_UNIMPLEMENTED();
	}



	NGARenderTargetView::NGARenderTargetView(NGARenderTargetView&& view) noexcept :
		mDescriptorHandle(view.mDescriptorHandle)
	{
		view.mDescriptorHandle.Clear();
	}

	bool NGARenderTargetView::Construct(const NGATexture& texture, int slice)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGARenderTargetView::Construct(const NGASwapChain& swapChain, int buffer)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(swapChain.IsConstructed(), false);

		ID3D12Resource* backBuffer;
		HRESULT hr = swapChain.mSwapChain->GetBuffer(buffer, IID_PPV_ARGS(&backBuffer));
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to get back buffer. HRESULT %X", hr);

		mDescriptorHandle = NgaDx12State.mRtvHeap.Allocate();
		NgaDx12State.mDevice->CreateRenderTargetView(backBuffer, nullptr, mDescriptorHandle);
		
		COM_SAFE_RELEASE(backBuffer);

		return true;
	}

	void NGARenderTargetView::Destruct()
	{
		mDescriptorHandle.Clear();
	}

	bool NGARenderTargetView::IsConstructed()const
	{
		return mDescriptorHandle.IsValid();
	}

	bool NGARenderTargetView::operator==(const NGARenderTargetView& other)const
	{
		return mDescriptorHandle == other.mDescriptorHandle;
	}

	bool NGARenderTargetView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGARenderTargetView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		return this->operator==(view);
	}

	bool NGARenderTargetView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}



	NGADepthStencilView::NGADepthStencilView(NGADepthStencilView&& view) noexcept :
		mDescriptorHandle(view.mDescriptorHandle)
	{
		view.mDescriptorHandle.Clear();
	}

	bool NGADepthStencilView::Construct(const NGATexture& texture, int slice)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGADepthStencilView::Destruct()
	{
		mDescriptorHandle.Clear();
	}

	bool NGADepthStencilView::IsConstructed()const
	{
		return mDescriptorHandle.IsValid();
	}

	bool NGADepthStencilView::operator==(const NGADepthStencilView& other)const
	{
		return mDescriptorHandle == other.mDescriptorHandle;
	}

	bool NGADepthStencilView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGADepthStencilView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGADepthStencilView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		return this->operator==(view);
	}
}

#endif