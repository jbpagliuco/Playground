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

		HRESULT hr = swapChain.mSwapChain->GetBuffer(buffer, IID_PPV_ARGS(&mResource));
		if (FAILED(hr)) {
			CORE_ERROR("Failed to get back buffer. (%X)", hr);
			return false;
		}

		mDescriptorHandle = NgaDx12State.mRtvHeap.Allocate();
		NgaDx12State.mDevice->CreateRenderTargetView(mResource, nullptr, mDescriptorHandle);
		
		return true;
	}

	void NGARenderTargetView::Destruct()
	{
		COM_SAFE_RELEASE(mResource);
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
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(texture.IsConstructed(), false);

		const NGATextureDesc& textureDesc = texture.GetDesc();

		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};

		// Convert typeless format to strict format
		if (NGAFormatIsTypeless(textureDesc.mFormat)) {
			desc.Format = NGATypelessFormatToDepthDXGI(textureDesc.mFormat);
		}
		else {
			desc.Format = NGAFormatToDXGI(textureDesc.mFormat);
		}

		switch (textureDesc.mType) {
		case NGATextureType::TEXTURE1D:
			if (texture.IsArray()) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
				desc.Texture1DArray.MipSlice = 0;
				desc.Texture1DArray.ArraySize = 1;
				desc.Texture1DArray.FirstArraySlice = D3D12CalcSubresource(0, slice, 0, 1, 1);
			}
			else {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
			}
			break;

		case NGATextureType::TEXTURE2D:
			if (texture.IsArray()) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.MipSlice = 0;
				desc.Texture2DArray.ArraySize = 1;
				desc.Texture1DArray.FirstArraySlice = D3D12CalcSubresource(0, slice, 0, 1, 1);
			}
			else {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			}
			break;

		default:
			desc.ViewDimension = D3D12_DSV_DIMENSION_UNKNOWN;
			break;
		}

		mDescriptorHandle = NgaDx12State.mDsvHeap.Allocate();
		NgaDx12State.mDevice->CreateDepthStencilView(texture.mResource, &desc, mDescriptorHandle);

		return true;
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