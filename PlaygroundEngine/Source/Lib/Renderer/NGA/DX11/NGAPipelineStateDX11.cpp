#include "NGA/NGAPipelineState.h"

#if CORE_RENDER_API(DX11)

#include "NGA/NGAShader.h"
#include "NGACoreInternalDX11.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAPipelineState);

	static ID3D11RasterizerState* Dx11CreateRasterizerState(const NGARasterizerStateDesc &desc)
	{
		ID3D11RasterizerState *out = nullptr;

		D3D11_RASTERIZER_DESC rs{};
		rs.AntialiasedLineEnable = desc.mAntialias;
		rs.CullMode = (D3D11_CULL_MODE)desc.mCullMode;
		rs.DepthBias = 0;
		rs.DepthBiasClamp = 0.0f;
		rs.DepthClipEnable = true;
		rs.FillMode = (D3D11_FILL_MODE)desc.mFillMode;
		rs.FrontCounterClockwise = desc.mFrontCounterClockwise;
		rs.MultisampleEnable = false;
		rs.ScissorEnable = false;
		rs.SlopeScaledDepthBias = 0.0f;

		HRESULT hr = NgaDx11State.mDevice->CreateRasterizerState(&rs, &out);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), nullptr, "ID3D11Device::CreateRasterizerState() failed with HRESULT %X", hr);

		return out;
	}

	static ID3D11DepthStencilState* Dx11CreateDepthStencilState(const NGADepthStencilStateDesc &desc)
	{
		ID3D11DepthStencilState *out = nullptr;

		D3D11_DEPTH_STENCIL_DESC ds{};
		ds.DepthEnable = true;
		ds.DepthFunc = (D3D11_COMPARISON_FUNC)desc.mDepthFunc;
		ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		ds.StencilEnable = false;

		HRESULT hr = NgaDx11State.mDevice->CreateDepthStencilState(&ds, &out);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), nullptr, "ID3D11Device::CreateDepthStencilState() failed with HRESULT %X", hr);

		return out;
	}

	static ID3D11InputLayout* Dx11CreateInputLayout(const NGAVertexFormatDesc &desc, const NGAShader *vertexShader)
	{
		ID3D11InputLayout *out;

		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;
		for (auto &attr : desc.mAttributes) {
			D3D11_INPUT_ELEMENT_DESC element;
			element.InputSlot = 0;
			element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0;

			element.Format = NGAFormatToDXGI(attr.mFormat);
			element.SemanticName = GetSemanticName(attr.mSemanticType);
			element.SemanticIndex = attr.mSemanticIndex;
			element.AlignedByteOffset = attr.mOffset;

			elementDescs.push_back(element);
		}

		elementDescs[0].AlignedByteOffset = 0;

		HRESULT hr = NgaDx11State.mDevice->CreateInputLayout(&(elementDescs[0]), (UINT)elementDescs.size(), vertexShader->GetBytecode(), vertexShader->GetBytecodeSize(), &out);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), nullptr, "ID3D11Device::CreateInputLayout failed with HRESULT %X", hr);

		return out;
	}

	bool NGAPipelineState::Construct(const NGAFixedFunctionStateDesc &fixedFunctionDesc, const NGAGraphicsPipelineInputAssemblyDesc &inputAssemblyDesc)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);

		mPrimitiveTopology = inputAssemblyDesc.mPrimitiveTopology;

		mRasterizerState = Dx11CreateRasterizerState(fixedFunctionDesc.mRasterizerStateDesc);
		mDepthStencilState = Dx11CreateDepthStencilState(fixedFunctionDesc.mDepthStencilStateDesc);

		return true;
	}

	void NGAPipelineState::Destruct()
	{
		COM_SAFE_RELEASE(mRasterizerState);
		COM_SAFE_RELEASE(mDepthStencilState);
	}

	bool NGAPipelineState::IsConstructed()const
	{
		return mRasterizerState != nullptr || mDepthStencilState != nullptr;
	}
}

#endif