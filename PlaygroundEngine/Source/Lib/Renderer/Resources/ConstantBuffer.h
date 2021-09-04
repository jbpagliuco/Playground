#pragma once

#include "Renderer/NGA/NGAResources.h"

namespace playground
{
	enum class ConstantBufferUsage
	{
		IMMUTABLE = 0,
		GPU_WRITE = 1,
		CPU_WRITE = 2
	};

	class ConstantBuffer
	{
	public:
		bool Initialize(ConstantBufferUsage usage, void *initialData, size_t size);
		void Shutdown();

		void Map(void *pData);

		const NGABuffer& GetBuffer()const;
		size_t GetSize()const;

	private:
		NGABuffer mBuffer;

		ConstantBufferUsage mUsage;
		size_t mSize;
	};
}