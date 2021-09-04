#pragma once

#include "NGA/NGAResources.h"

namespace playground
{
	class VertexBuffer
	{
	public:
		bool Initialize(void *vertexData, size_t numVertices, size_t vertexStride);
		void Shutdown();

		const NGABuffer& GetBuffer()const;
		size_t GetNumVertices()const;
		size_t GetVertexStride()const;

	private:
		NGABuffer mBuffer;
		size_t mNumVertices;
		size_t mVertexStride;
	};
}