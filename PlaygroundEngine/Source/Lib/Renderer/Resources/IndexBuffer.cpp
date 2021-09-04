#include "IndexBuffer.h"

#include "Renderer.h"

namespace playground
{
	bool IndexBuffer::Initialize(IndexType *indexData, size_t numIndices)
	{
		CORE_ASSERT_RETURN_VALUE(!mBuffer.IsConstructed(), false);

		CORE_ASSERT_RETURN_VALUE(indexData != nullptr, false);
		CORE_ASSERT_RETURN_VALUE(numIndices > 0, false);

		mNumIndices = numIndices;

		NGABufferDesc desc;
		desc.mUsage = NGA_BUFFER_USAGE_INDEX;
		desc.mSizeInBytes = (uint32_t)numIndices * sizeof(IndexType);
		const bool success = mBuffer.Construct(desc, indexData);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to construct index buffer.");

		return true;
	}

	void IndexBuffer::Shutdown()
	{
		mBuffer.Destruct();
	}
	
	const NGABuffer& IndexBuffer::GetBuffer()const
	{
		return mBuffer;
	}

	size_t IndexBuffer::GetNumIndices()const
	{
		return mNumIndices;
	}

	size_t IndexBuffer::GetIndexSize()const
	{
		return sizeof(IndexType);
	}
}