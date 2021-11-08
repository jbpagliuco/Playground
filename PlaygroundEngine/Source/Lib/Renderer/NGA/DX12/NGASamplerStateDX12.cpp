#include "NGA/NGASamplerState.h"

#if CORE_RENDER_API(DX12)

#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGASamplerState);

	bool NGASamplerState::Construct(const NGASamplerStateDesc& desc)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);

		// We're only filling out the description here. The pipeline root signature will take of creating the actual sampler.
		mSamplerDesc.Filter = (D3D12_FILTER)desc.mFilter;
		mSamplerDesc.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)desc.mAddressU;
		mSamplerDesc.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)desc.mAddressV;
		mSamplerDesc.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)desc.mAddressW;
		mSamplerDesc.MipLODBias = 0.0f;
		mSamplerDesc.MaxAnisotropy = 1;
		mSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		mSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		mSamplerDesc.MinLOD = 0;
		mSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

		mConstructed = true;

		return true;
	}

	void NGASamplerState::Destruct()
	{
	}

	bool NGASamplerState::IsConstructed()const
	{
		return mConstructed;
	}
}

#endif