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

		// Map data to this buffer.
		void Map(const void *pData, size_t size);

		template <typename T>
		void Map(const T* pData)
		{
			Map(pData, sizeof(T));
		}

		const NGABuffer& GetBuffer()const;
		size_t GetSize()const;

	private:
		NGABuffer mBuffer;

		ConstantBufferUsage mUsage;
		size_t mSize;
	};
}