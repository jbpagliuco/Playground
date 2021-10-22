#include "NGA/NGAResourceViews.h"

#if CORE_RENDER_API(DX11)

#include "NGACoreInternalDX11.h"

#include "NGA/NGAResources.h"
#include "NGA/NGASwapChain.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAShaderResourceView);
	NGA_GPU_CLASS_IMPLEMENT(NGARenderTargetView);
	NGA_GPU_CLASS_IMPLEMENT(NGADepthStencilView);


	template <typename T1, typename T2>
	static bool ViewsPointToSameResource(T1* view1, T2* view2)
	{
		if (view1 == nullptr || view2 == nullptr) {
			return false;
		}

		ID3D11Resource* resource1;
		view1->GetResource(&resource1);

		ID3D11Resource* resource2;
		view2->GetResource(&resource2);

		const bool same = resource1 == resource2;

		resource1->Release();
		resource2->Release();

		return same;
	}


	bool NGAShaderResourceView::Construct(const NGATexture &texture)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(texture.IsConstructed(), false);

		HRESULT hr;

		const NGATextureDesc &texDesc = texture.GetDesc();
		if (!NGAFormatIsTypeless(texDesc.mFormat)) {
			// We can just create the view without any description.
			hr = NgaDx11State.mDevice->CreateShaderResourceView(texture.mResource, nullptr, &mView);
		}
		else {
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};

			desc.Format = NGATypelessFormatToColorDXGI(texture.mDesc.mFormat);

			switch (texDesc.mType) {
			case NGATextureType::TEXTURE2D:
			{
				if (texture.IsArray()) {
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Texture2DArray.ArraySize = texDesc.mArraySize;
					desc.Texture2DArray.MipLevels = -1;
				}
				else {
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MipLevels = -1;
				}
				break;
			}

			default:
				CORE_ASSERT_RETURN_VALUE(false, false, "Unimplemented texture type.");
			};

			hr = NgaDx11State.mDevice->CreateShaderResourceView(texture.mResource, &desc, &mView);
		};

		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateShaderResourceView() failed with HRESULT %X", hr);

		return true;
	}

	void NGAShaderResourceView::Destruct()
	{
		COM_SAFE_RELEASE(mView);
	}

	bool NGAShaderResourceView::IsConstructed()const
	{
		return mView != nullptr;
	}

	bool NGAShaderResourceView::operator==(const NGAShaderResourceView& other)const
	{
		return mView == other.mView;
	}

	bool NGAShaderResourceView::operator!=(const NGAShaderResourceView& other)const
	{
		return mView != other.mView;
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGAShaderResourceView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	void NGAShaderResourceView::GenerateMips()const
	{
		NgaDx11State.mContext->GenerateMips(mView);
	}



	NGARenderTargetView::NGARenderTargetView(NGARenderTargetView&& view) noexcept :
		mView(view.mView)
	{
		view.mView = nullptr;
	}

	bool NGARenderTargetView::Construct(const NGATexture &texture, int slice)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(texture.IsConstructed(), false);

		const NGATextureDesc &textureDesc = texture.GetDesc();

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = NGAFormatToDXGI(textureDesc.mFormat);

		switch (textureDesc.mType) {
		case NGATextureType::TEXTURE1D:
			if (texture.IsArray()) {
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
				renderTargetViewDesc.Texture1DArray.MipSlice = 0;
				renderTargetViewDesc.Texture1DArray.ArraySize = 1;
				renderTargetViewDesc.Texture1DArray.FirstArraySlice = D3D11CalcSubresource(0, slice, 1);
			} 
			else {
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			}
			break;

		case NGATextureType::TEXTURE2D:
			if (texture.IsArray()) {
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				renderTargetViewDesc.Texture2DArray.MipSlice = 0;
				renderTargetViewDesc.Texture2DArray.ArraySize = 1;
				renderTargetViewDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, slice, 1);
			} 
			else {
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			}
			break;

		case NGATextureType::TEXTURE3D:
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			break;

		default:
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
			break;
		}

		// Create the render target view.
		HRESULT hr = NgaDx11State.mDevice->CreateRenderTargetView(texture.mResource, &renderTargetViewDesc, &mView);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create render target view.");

		return true;
	}

	bool NGARenderTargetView::Construct(const NGASwapChain &swapChain, int buffer)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(swapChain.IsConstructed(), false);

		// TODO: Might be a better way to query the back buffer.
		ID3D11Texture2D *backBuffer;
		HRESULT hr = swapChain.mSwapChain->GetBuffer(buffer, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to get back buffer. HRESULT %X", hr);

		hr = NgaDx11State.mDevice->CreateRenderTargetView(backBuffer, nullptr, &mView);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateRenderTargetView() failed with HRESULT %X", hr);

		COM_SAFE_RELEASE(backBuffer);

		return true;
	}

	void NGARenderTargetView::Destruct()
	{
		COM_SAFE_RELEASE(mView);
	}

	bool NGARenderTargetView::IsConstructed()const
	{
		return mView != nullptr;
	}

	bool NGARenderTargetView::operator==(const NGARenderTargetView& other)const
	{
		return mView == other.mView;
	}

	bool NGARenderTargetView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGARenderTargetView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGARenderTargetView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}



	NGADepthStencilView::NGADepthStencilView(NGADepthStencilView&& view) noexcept :
		mView(view.mView)
	{
		view.mView = nullptr;
	}

	bool NGADepthStencilView::Construct(const NGATexture& texture, int slice)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(texture.IsConstructed(), false);

		const NGATextureDesc &textureDesc = texture.GetDesc();
	
		D3D11_DEPTH_STENCIL_VIEW_DESC desc{};

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
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
				desc.Texture1DArray.MipSlice = 0;
				desc.Texture1DArray.ArraySize = 1;
				desc.Texture1DArray.FirstArraySlice = D3D11CalcSubresource(0, slice, 1);
			} 
			else {
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			}
			break;

		case NGATextureType::TEXTURE2D:
			if (texture.IsArray()) {
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.MipSlice = 0;
				desc.Texture2DArray.ArraySize = 1;
				desc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, slice, 1);
			} 
			else {
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
			break;

		default:
			desc.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
			break;
		}

		HRESULT hr = NgaDx11State.mDevice->CreateDepthStencilView(texture.mResource, &desc, &mView);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateDepthStencilView() failed with HRESULT %X", hr);

		return true;
	}

	void NGADepthStencilView::Destruct()
	{
		COM_SAFE_RELEASE(mView);
	}

	bool NGADepthStencilView::IsConstructed()const
	{
		return mView != nullptr;
	}

	bool NGADepthStencilView::PointsToSameResource(const NGAShaderResourceView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGADepthStencilView::PointsToSameResource(const NGARenderTargetView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGADepthStencilView::PointsToSameResource(const NGADepthStencilView& view)const
	{
		return ViewsPointToSameResource(mView, view.mView);
	}

	bool NGADepthStencilView::operator==(const NGADepthStencilView& other)const
	{
		return mView == other.mView;
	}
}

#endif