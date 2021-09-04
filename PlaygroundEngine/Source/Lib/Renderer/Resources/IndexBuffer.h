#pragma once

#include "NGA/NGAResources.h"

namespace playground
{
	typedef uint32_t IndexType;

	class IndexBuffer
	{
	public:
		bool Initialize(IndexType *indexData, size_t numIndices);
		void Shutdown();
		
		const NGABuffer& GetBuffer()const;
		size_t GetNumIndices()const;
		size_t GetIndexSize()const;

	private:
		NGABuffer mBuffer;
		size_t mNumIndices;
	};
}