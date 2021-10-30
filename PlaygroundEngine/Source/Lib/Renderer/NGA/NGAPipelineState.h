#pragma once

#include "NGA/NGACommon.h"
#include "NGA/NGAPipelineStateDefs.h"

namespace playground
{
	class NGAInputLayout;
	class NGAShader;

	struct NGARasterizerStateDesc
	{
		NGACullMode mCullMode = NGACullMode::CULL_BACK;
		NGAFillMode mFillMode = NGAFillMode::SOLID;
		bool mAntialias = false;
		bool mFrontCounterClockwise = false;
	};

	struct NGADepthStencilStateDesc
	{
		NGADepthFunc mDepthFunc = NGADepthFunc::LESS;
	};

	class NGAPipelineStateDesc
	{
	public:
		// Shaders
		const NGAShader* mVertexShader = nullptr;
		const NGAShader* mPixelShader = nullptr;

		// Fixed function configuration
		NGARasterizerStateDesc mRasterizerState;
		NGADepthStencilStateDesc mDepthStencilState;

		// Input assembler
		NGAPrimitiveTopology mPrimitiveTopology = NGAPrimitiveTopology::TRIANGLE_LIST;
		NGAVertexFormatDesc mVertexFormat;

		// Formats
		NGAFormat mRTVFormat;
		NGAFormat mDSVFormat;
	};

	class NGAPipelineState
	{
		NGA_GPU_CLASS(NGAPipelineState);

	public:
		bool Construct(const NGAPipelineStateDesc &desc);
		void Destruct();

		bool IsConstructed()const;

#if CORE_RENDER_API(DX11)
	private:
		NGAPrimitiveTopology mPrimitiveTopology;

		struct ID3D11RasterizerState *mRasterizerState = nullptr;
		struct ID3D11DepthStencilState *mDepthStencilState = nullptr;
#elif CORE_RENDER_API(DX12)
		ID3D12PipelineState* mPSO = nullptr;
		ID3D12RootSignature* mRootSignature = nullptr;
#endif

		friend class NGACommandContext;
	};
}