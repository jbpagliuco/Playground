#pragma once

#include <vector>

#include "NGA/NGACommon.h"

namespace playground
{
	class NGAShader;

	class NGAInputLayout
	{
		NGA_GPU_CLASS(NGAInputLayout);

	public:
		bool Construct(const NGAVertexFormatDesc &vertexFormatDesc, const NGAShader &vertexShader);
		void Destruct();

		bool IsConstructed()const;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11InputLayout *mInputLayout;
#endif

		friend class NGACommandContext;
	};
}