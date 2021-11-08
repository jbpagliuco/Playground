#pragma once

#include "NGA/NGABlob.h"
#include "NGA/NGACommon.h"

namespace playground
{
	enum class NGAShaderType
	{
		VERTEX = 0,
		PIXEL
	};

	class NGAShader
	{
		NGA_GPU_CLASS(NGAShader);

	public:
		bool Construct(const char *filename, NGAShaderType type);
		void Destruct();

		bool IsConstructed()const;

		void* GetBytecode()const;
		size_t GetBytecodeSize()const;

		NGAShaderType GetType()const;

#if CORE_RENDER_API(DX12)
		operator D3D12_SHADER_BYTECODE()const;
#endif

	private:
		NGAShaderType mType;
		NGABlob mBytecode;

#if CORE_RENDER_API(DX11)
	private:
		union {
			struct ID3D11VertexShader *mVertexShader;
			struct ID3D11PixelShader *mPixelShader;
		};
#endif

		friend class NGACommandContext;
		friend class NGAPipelineState;
	};
}