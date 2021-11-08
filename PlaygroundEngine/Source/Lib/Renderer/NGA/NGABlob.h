#pragma once

#include "NGA/NGACommon.h"

namespace playground
{
	class NGABlob
	{
		NGA_GPU_CLASS(NGABlob);

	public:
		bool Construct(size_t size, void* data = nullptr);
		void Destruct();

		bool IsConstructed()const;

		void* GetBufferPointer()const;
		size_t GetBufferSize()const;

#if CORE_RENDER_API(DX12)
	public:
		ID3DBlob*& GetBlob();
#endif


#if CORE_RENDER_API(DX12)
	private:
		ID3DBlob* mBlob;
#endif

		friend class NGACommandContext;
	};
}