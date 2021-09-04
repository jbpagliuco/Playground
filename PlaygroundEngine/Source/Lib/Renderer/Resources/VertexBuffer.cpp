#include "VertexBuffer.h"

#include "Core/Util/Util.h"

#include "Renderer.h"

namespace playground
{
	bool VertexBuffer::Initialize(void *vertexData, size_t numVertices, size_t vertexStride)
	{
		CORE_ASSERT_RETURN_VALUE(!mBuffer.IsConstructed(), false);

		CORE_ASSERT_RETURN_VALUE(vertexData != nullptr, false);
		CORE_ASSERT_RETURN_VALUE(numVertices > 0, false);
		CORE_ASSERT_RETURN_VALUE(vertexStride > 0, false);

		mNumVertices = numVertices;
		mVertexStride = vertexStride;

		NGABufferDesc desc;
		desc.mUsage = NGA_BUFFER_USAGE_VERTEX;
		desc.mSizeInBytes = (uint32_t)numVertices * (uint32_t)vertexStride;
		const bool success = mBuffer.Construct(desc, vertexData);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to construct vertex buffer.");

		return true;
	}

	void VertexBuffer::Shutdown()
	{
		mBuffer.Destruct();
	}

	const NGABuffer& VertexBuffer::GetBuffer()const
	{
		return mBuffer;
	}

	size_t VertexBuffer::GetNumVertices()const
	{
		return mNumVertices;
	}
	
	size_t VertexBuffer::GetVertexStride()const
	{
		return mVertexStride;
	}

}