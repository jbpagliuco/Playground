#include "ConstantBuffer.h"

#include "Core/Util/Util.h"
#include "Renderer/Renderer.h"

namespace playground
{
	bool ConstantBuffer::Initialize(ConstantBufferUsage usage, void *initialData, size_t size)
	{
		CORE_ASSERT_RETURN_VALUE(!mBuffer.IsConstructed(), false);
		
		NGABufferDesc desc;
		desc.mUsage = NGA_BUFFER_USAGE_CONSTANT;
		desc.mSizeInBytes = RoundToNearestMultiple((int)size, 16);

		switch (usage) {
		case ConstantBufferUsage::IMMUTABLE:
			CORE_ASSERT_RETURN_VALUE(initialData != nullptr, false, "Immutable buffers needs data upon creation.");
			break;

		case ConstantBufferUsage::GPU_WRITE:
			desc.mUsage |= NGA_BUFFER_USAGE_GPU_WRITE;
			break;

		case ConstantBufferUsage::CPU_WRITE:
			desc.mUsage |= NGA_BUFFER_USAGE_CPU_WRITE;
			break;
		};

		mBuffer.Construct(desc, initialData);
		mUsage = usage;
		mSize = size;

		return true;
	}

	void ConstantBuffer::Shutdown()
	{
		mBuffer.Destruct();
	}

	void ConstantBuffer::Map(const void *pData, size_t size)
	{
		CORE_ASSERT_RETURN(mUsage == ConstantBufferUsage::CPU_WRITE, "Trying to map data to a non-CPU writeable buffer.");
		Playground_RendererStateManager->MapBufferData(mBuffer, pData, size);
	}

	const NGABuffer& ConstantBuffer::GetBuffer()const
	{
		return mBuffer;
	}

	size_t ConstantBuffer::GetSize()const
	{
		return mSize;
	}
}