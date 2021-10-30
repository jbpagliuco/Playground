#include "NGA/NGAResources.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGATexture);
	NGA_GPU_CLASS_IMPLEMENT(NGABuffer);


	bool NGABuffer::IsVertexBuffer()const
	{
		return mDesc.mUsage & NGABufferUsage::NGA_BUFFER_USAGE_VERTEX;
	}

	bool NGABuffer::IsIndexBuffer()const
	{
		return mDesc.mUsage & NGABufferUsage::NGA_BUFFER_USAGE_INDEX;
	}

	bool NGABuffer::IsConstantBuffer()const
	{
		return mDesc.mUsage & NGABufferUsage::NGA_BUFFER_USAGE_CONSTANT;
	}

	size_t NGABuffer::GetVertexStride()const
	{
		CORE_ASSERT(IsVertexBuffer(), "Tried to get index buffer format for a non-index buffer.");

		return mDesc.mVertexStride;
	}

	NGAFormat NGABuffer::GetIndexBufferFormat()const
	{
		CORE_ASSERT(IsIndexBuffer(), "Tried to get index buffer format for a non-index buffer.");

		return mDesc.mIndexFormat;
	}
}