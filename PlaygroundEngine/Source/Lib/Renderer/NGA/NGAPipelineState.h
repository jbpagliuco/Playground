#pragma once

#include "NGA/NGACommon.h"
#include "NGA/NGAPipelineStateDefs.h"

namespace playground
{
	struct NGAGraphicsPipelineInputAssemblyDesc
	{
		NGAPrimitiveTopology mPrimitiveTopology;
	};

	struct NGARasterizerStateDesc
	{
		bool mAntialias = false;
		NGACullMode mCullMode = NGACullMode::CULL_BACK;
		NGAFillMode mFillMode = NGAFillMode::SOLID;
		bool mFrontCounterClockwise = false;
	};

	struct NGADepthStencilStateDesc
	{
		NGADepthFunc mDepthFunc = NGADepthFunc::LESS;
	};

	struct NGAFixedFunctionStateDesc
	{
		NGARasterizerStateDesc mRasterizerStateDesc;
		NGADepthStencilStateDesc mDepthStencilStateDesc;
	};

	class NGAPipelineState
	{
		NGA_GPU_CLASS(NGAPipelineState);

	public:
		bool Construct(const NGAFixedFunctionStateDesc &fixedFunctionDesc, const NGAGraphicsPipelineInputAssemblyDesc &inputAssemblyDesc);
		void Destruct();

		bool IsConstructed()const;

#if CORE_RENDER_API(DX11)
	private:
		NGAPrimitiveTopology mPrimitiveTopology;

		struct ID3D11RasterizerState *mRasterizerState = nullptr;
		struct ID3D11DepthStencilState *mDepthStencilState = nullptr;
#endif

		friend class NGACommandContext;
	};
}