#include "NGA/NGASamplerState.h"

#if CORE_RENDER_API(DX11)

#include "NGACoreInternalDX11.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGASamplerState);

	bool NGASamplerState::Construct(const NGASamplerStateDesc &desc)
	{
		D3D11_SAMPLER_DESC dxdesc;
		dxdesc.Filter = (D3D11_FILTER)desc.mFilter;
		dxdesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)desc.mAddressU;
		dxdesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)desc.mAddressV;
		dxdesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)desc.mAddressW;
		dxdesc.MipLODBias = 0.0f;
		dxdesc.MaxAnisotropy = 1;
		dxdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		dxdesc.BorderColor[0] = desc.mBorderColor.r;
		dxdesc.BorderColor[1] = desc.mBorderColor.g;
		dxdesc.BorderColor[2] = desc.mBorderColor.b;
		dxdesc.BorderColor[3] = desc.mBorderColor.a;
		dxdesc.MinLOD = 0;
		dxdesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = NgaDx11State.mDevice->CreateSamplerState(&dxdesc, &mSamplerState);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateSamplerState() failed with HRESULT %X", hr);

		return true;
	}

	void NGASamplerState::Destruct()
	{
		COM_SAFE_RELEASE(mSamplerState);
	}

	bool NGASamplerState::IsConstructed()const
	{
		return mSamplerState != nullptr;
	}
}

#endif