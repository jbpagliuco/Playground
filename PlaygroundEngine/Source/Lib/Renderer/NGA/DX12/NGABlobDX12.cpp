#include "NGA/NGABlob.h"

#if CORE_RENDER_API(DX12)

#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGABlob);

	bool NGABlob::Construct(size_t size, void* data)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false);
		CORE_ASSERT_RETURN_VALUE(size > 0, false);

		// Copy into a CPU blob.
		D3DCreateBlob(size, &mBlob);

		if (data != nullptr) {
			CopyMemory(mBlob->GetBufferPointer(), data, size);
		}

		return true;
	}

	void NGABlob::Destruct()
	{
		COM_SAFE_RELEASE(mBlob);
	}

	bool NGABlob::IsConstructed()const
	{
		return mBlob != nullptr;
	}

	void* NGABlob::GetBufferPointer()const
	{
		CORE_ASSERT_RETURN_VALUE(mBlob != nullptr, nullptr);
		return mBlob->GetBufferPointer();
	}

	size_t NGABlob::GetBufferSize()const
	{
		CORE_ASSERT_RETURN_VALUE(mBlob != nullptr, -1);
		return mBlob->GetBufferSize();
	}

	ID3DBlob*& NGABlob::GetBlob()
	{
		return mBlob;
	}
}

#endif // CORE_RENDER_API(DX12)