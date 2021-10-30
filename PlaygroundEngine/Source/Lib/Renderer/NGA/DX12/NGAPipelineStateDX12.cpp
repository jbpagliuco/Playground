#include "NGA/NGAPipelineState.h"

#if CORE_RENDER_API(DX12)

#include "ReflectionRegistry.h"

#include "Core/Util/Util.h"
#include "NGA/NGAShader.h"
#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGAPipelineState);

	static HRESULT CreateRootSignature(ID3DBlob*& serializedRootSignature, ID3DBlob*& errorBlob)
	{
		constexpr int NUM_CONSTANT_BUFFERS = 4;
		constexpr int NUM_SHADER_RESOURCE_VIEWS = 4;

		CD3DX12_ROOT_PARAMETER rootParameters[NUM_CONSTANT_BUFFERS + 1];

		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);
		rootParameters[2].InitAsConstantBufferView(2);
		rootParameters[3].InitAsConstantBufferView(3);

		CD3DX12_DESCRIPTOR_RANGE srvRange;
		srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, NUM_SHADER_RESOURCE_VIEWS, 0);
		rootParameters[4].InitAsDescriptorTable(1, &srvRange);

		CD3DX12_STATIC_SAMPLER_DESC staticSamplerDescs[NUM_SHADER_RESOURCE_VIEWS];
		staticSamplerDescs[0].Init(0);
		staticSamplerDescs[1].Init(1);
		staticSamplerDescs[2].Init(2);
		staticSamplerDescs[3].Init(3);

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters = STATIC_ARRAY_SIZE(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumStaticSamplers = STATIC_ARRAY_SIZE(staticSamplerDescs);
		rootSignatureDesc.pStaticSamplers = staticSamplerDescs;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		return D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
	}

	bool NGAPipelineState::Construct(const NGAPipelineStateDesc& pipelineDesc)
	{
		// Sanity checks
		CORE_ASSERT_RETURN_VALUE(pipelineDesc.mVertexShader != nullptr, false, "Invalid vertex shader.");
		CORE_ASSERT_RETURN_VALUE(pipelineDesc.mPixelShader != nullptr, false, "Invalid pixel shader.");
		CORE_ASSERT_RETURN_VALUE(pipelineDesc.mVertexFormat.mAttributes.size() > 0, false, "Invalid vertex format.");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
		desc.pRootSignature = nullptr;

		ID3DBlob* serializedRootSignature = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = CreateRootSignature(serializedRootSignature, errorBlob);
		if (FAILED(hr)) {
			CORE_ASSERT(false, "Failed to create root signature with error message:\r\n\r\n%.*s", errorBlob->GetBufferSize(), errorBlob->GetBufferPointer());
			return false;
		}

		hr = NgaDx12State.mDevice->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false);
		desc.pRootSignature = mRootSignature;

		// Shaders
		desc.VS = (D3D12_SHADER_BYTECODE)*pipelineDesc.mVertexShader;
		desc.PS = (D3D12_SHADER_BYTECODE)*pipelineDesc.mPixelShader;
		
		// Blend state
		desc.BlendState.AlphaToCoverageEnable = false;
		desc.BlendState.IndependentBlendEnable = false;
		for (int i = 0; i < 8; ++i) {
			desc.BlendState.RenderTarget[i].BlendEnable = false;
			desc.BlendState.RenderTarget[i].LogicOpEnable = false;
			desc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			desc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
			desc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			desc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			desc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			desc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			desc.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
			desc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		// Sample mask
		desc.SampleMask = 0xffffffff;

		// Rasterizer state
		desc.RasterizerState.CullMode = (D3D12_CULL_MODE)pipelineDesc.mRasterizerState.mCullMode;
		desc.RasterizerState.FillMode = (D3D12_FILL_MODE)pipelineDesc.mRasterizerState.mFillMode;
		desc.RasterizerState.AntialiasedLineEnable = pipelineDesc.mRasterizerState.mAntialias;
		desc.RasterizerState.FrontCounterClockwise = pipelineDesc.mRasterizerState.mFrontCounterClockwise;
		desc.RasterizerState.DepthBias = 0;
		desc.RasterizerState.DepthBiasClamp = 0.0f;
		desc.RasterizerState.DepthClipEnable = true;
		desc.RasterizerState.MultisampleEnable = false;
		desc.RasterizerState.SlopeScaledDepthBias = 0.0f;
		desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// Depth stencil state
		desc.DepthStencilState.DepthFunc = (D3D12_COMPARISON_FUNC)pipelineDesc.mDepthStencilState.mDepthFunc;
		desc.DepthStencilState.DepthEnable = true;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.StencilEnable = false;

		// Input layout
		const refl::Enum* semanticTypeEnum = refl::GetSystemRegistry().GetEnum("playground::NGAVertexSemanticType");
		std::vector<D3D12_INPUT_ELEMENT_DESC> elementDescs;
		std::vector<std::string> semanticNames; // gross...
		for (auto& attr : pipelineDesc.mVertexFormat.mAttributes) {
			D3D12_INPUT_ELEMENT_DESC element;
			element.InputSlot = 0;
			element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0;

			element.Format = NGAFormatToDXGI(attr.mFormat);
			semanticNames.push_back(semanticTypeEnum->GetValueString((int8_t)attr.mSemanticType));
			element.SemanticName = semanticNames.back().c_str();
			element.SemanticIndex = attr.mSemanticIndex;
			element.AlignedByteOffset = attr.mOffset;

			elementDescs.push_back(element);
		}
		elementDescs[0].AlignedByteOffset = 0;

		desc.InputLayout.pInputElementDescs = &elementDescs[0];
		desc.InputLayout.NumElements = elementDescs.size();
		desc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)pipelineDesc.mPrimitiveTopology;

		desc.NumRenderTargets = 1;
		for (auto& format : desc.RTVFormats) {
			format = DXGI_FORMAT_UNKNOWN;
		}
		desc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		hr = NgaDx12State.mDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPSO));

		COM_SAFE_RELEASE(serializedRootSignature);
		COM_SAFE_RELEASE(errorBlob);

		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create PSO. (%X)", hr);

		return true;
	}

	void NGAPipelineState::Destruct()
	{
		COM_SAFE_RELEASE(mRootSignature);
		COM_SAFE_RELEASE(mPSO);
	}

	bool NGAPipelineState::IsConstructed()const
	{
		return mPSO != nullptr;
	}
}

#endif