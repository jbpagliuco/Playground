#include "NGA/NGAResources.h"

#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)
#include "Renderer/NGA/Common/NGADirectXUtil.h"
#endif

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGATexture);
	NGA_GPU_CLASS_IMPLEMENT(NGABuffer);



	bool NGATexture::IsArray()const
	{
		return mDesc.mArraySize > 1;
	}

	const NGATextureDesc& NGATexture::GetDesc()const
	{
		return mDesc;
	}



	const NGABufferDesc& NGABuffer::GetDesc()const
	{
		return mDesc;
	}

	NGABufferUsage NGABuffer::GetBufferType()const
	{
		if (IsVertexBuffer()) {
			return NGA_BUFFER_USAGE_VERTEX;
		}
		if (IsIndexBuffer()) {
			return NGA_BUFFER_USAGE_INDEX;
		}
		if (IsConstantBuffer()) {
			return NGA_BUFFER_USAGE_CONSTANT;
		}

		CORE_UNIMPLEMENTED();
		return NGA_BUFFER_USAGE_NONE;
	}

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

	size_t NGABuffer::GetBufferElementSize()const
	{
#if CORE_RENDER_API(DX11) || CORE_RENDER_API(DX12)
		return IsConstantBuffer() ? CalcConstantBufferByteSize(mDesc.mSizeInBytes) : mDesc.mSizeInBytes;
#endif
	}

	size_t NGABuffer::GetBufferSize()const
	{
		return GetBufferElementSize() * mDesc.mArraySize;
	}

	bool NGABuffer::IsCPUWritable()const
	{
		return (mDesc.mUsage & NGA_BUFFER_USAGE_CPU_WRITE) || (mDesc.mUsage & NGA_BUFFER_USAGE_CPU_READ_WRITE);
	}
}