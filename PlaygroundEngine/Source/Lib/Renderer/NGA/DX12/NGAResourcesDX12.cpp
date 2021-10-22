#include "NGA/NGAResources.h"

#if CORE_RENDER_API(DX12)

#include "DirectXTK/Include/DDSTextureLoader.h"

#include "Core/Util/String.h"

#include "NGACoreInternalDX12.h"

namespace playground
{
	NGA_GPU_CLASS_IMPLEMENT(NGATexture);
	NGA_GPU_CLASS_IMPLEMENT(NGABuffer);


	
	bool NGATexture::Construct(const NGATextureDesc& desc, void* initialData)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGATexture::ConstructFromFile(const NGATextureDesc& desc, const char* filename)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGATexture::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGATexture::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGATexture::IsArray()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	const NGATextureDesc& NGATexture::GetDesc()const
	{
		return mDesc;
	}




	bool NGABuffer::Construct(const NGABufferDesc& desc)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	bool NGABuffer::Construct(const NGABufferDesc& desc, void* initialData)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGABuffer::Destruct()
	{
		CORE_UNIMPLEMENTED();
	}

	bool NGABuffer::IsConstructed()const
	{
		CORE_UNIMPLEMENTED();
		return false;
	}
}

#endif // CORE_RENDER_API(DX11)