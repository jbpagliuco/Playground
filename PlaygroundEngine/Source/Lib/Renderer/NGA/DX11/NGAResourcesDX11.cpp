#include "NGA/NGAResources.h"

#if CORE_RENDER_API(DX11)

#include "DirectXTK/Include/DDSTextureLoader.h"

#include "Core/Util/String.h"

#include "NGACoreInternalDX11.h"

namespace playground
{
	static UINT CreateBindFlags(const NGATextureDesc &desc)
	{
		UINT bindFlags = 0;

		bindFlags |= (desc.mBindFlags & NGA_TEXTURE_BIND_RENDER_TARGET) ? D3D11_BIND_RENDER_TARGET : 0;
		bindFlags |= (desc.mBindFlags & NGA_TEXTURE_BIND_SHADER_RESOURCE) ? D3D11_BIND_SHADER_RESOURCE : 0;
		bindFlags |= (desc.mBindFlags & NGA_TEXTURE_BIND_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0;
		
		return bindFlags;
	}

	bool NGATexture::Construct(const NGATextureDesc &desc, void *initialData)
	{
		CORE_ASSERT_RETURN_VALUE(desc.mWidth > 0 && desc.mHeight > 0, false, "Invalid texture dimension: (%d, %d).", desc.mWidth, desc.mHeight);
		CORE_ASSERT_RETURN_VALUE(desc.mBindFlags != NGA_TEXTURE_BIND_NONE, false, "Texture MUST be bound.");

		if (desc.mUsage == NGAUsage::IMMUTABLE) {
			CORE_ASSERT_RETURN_VALUE(initialData != nullptr, false, "Immutable textures MUST be provided with texture data.");
		}

		mDesc = desc;

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = desc.mWidth;
		textureDesc.Height =  desc.mHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = desc.mArraySize;
		textureDesc.Format = NGAFormatToDXGI(desc.mFormat);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = NGAUsageToD3D11(desc.mUsage);
		textureDesc.BindFlags = CreateBindFlags(desc);
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HRESULT hr;
		switch (desc.mType) {
		case NGATextureType::TEXTURE2D:
			hr = NgaDx11State.mDevice->CreateTexture2D(&textureDesc, NULL, (ID3D11Texture2D**)&mResource);
			break;

		default:
			CORE_FATAL_ERROR(false, "Unimplemented.");
		};

		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create texture.");

		return true;
	}

	bool NGATexture::ConstructFromFile(const NGATextureDesc &desc, const char *filename)
	{
		D3D11_USAGE usage = NGAUsageToD3D11(desc.mUsage);
		unsigned int bindFlags = CreateBindFlags(desc);
		unsigned int miscFlags = (desc.mType == NGATextureType::TEXTURECUBE) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(NgaDx11State.mDevice, StrToWideString(filename).c_str(),
			0, usage, bindFlags, 0, miscFlags, false, 
			&mResource, nullptr);

		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to load texture from file %s (HR: 0x%X)", filename, hr);

		mDesc = desc;

		if (mDesc.mType != NGATextureType::TEXTURECUBE) {
			D3D11_RESOURCE_DIMENSION texDimension;
			mResource->GetType(&texDimension);

			switch (texDimension) {
			case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			{
				mDesc.mType = NGATextureType::TEXTURE1D;

				ID3D11Texture1D* tex1d;
				mResource->QueryInterface(IID_ID3D11Texture1D, (void**)&tex1d);

				D3D11_TEXTURE1D_DESC tex1dDesc;
				tex1d->GetDesc(&tex1dDesc);
				mDesc.mFormat = DXGIFormatToNGA(tex1dDesc.Format);

				COM_SAFE_RELEASE(tex1d);

				break;
			}

			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			{
				mDesc.mType = NGATextureType::TEXTURE2D;

				ID3D11Texture2D* tex2d;
				mResource->QueryInterface(IID_ID3D11Texture2D, (void**)&tex2d);

				D3D11_TEXTURE2D_DESC tex2dDesc;
				tex2d->GetDesc(&tex2dDesc);
				mDesc.mFormat = DXGIFormatToNGA(tex2dDesc.Format);

				COM_SAFE_RELEASE(tex2d);

				break;
			}

			case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			{
				mDesc.mType = NGATextureType::TEXTURE3D;

				ID3D11Texture3D* tex3d;
				mResource->QueryInterface(IID_ID3D11Texture3D, (void**)&tex3d);

				D3D11_TEXTURE3D_DESC tex3dDesc;
				tex3d->GetDesc(&tex3dDesc);
				mDesc.mFormat = DXGIFormatToNGA(tex3dDesc.Format);

				COM_SAFE_RELEASE(tex3d);

				break;
			}

			default:
				CORE_FATAL_ERROR(false, "Unimplemented.");
			}
		}

		return true;
	}

	void NGATexture::Destruct()
	{
		COM_SAFE_RELEASE(mResource);
	}

	bool NGATexture::IsConstructed()const
	{
		return mResource != nullptr;
	}

	bool NGATexture::IsArray()const
	{
		return mDesc.mArraySize > 1;
	}

	const NGATextureDesc& NGATexture::GetDesc()const
	{
		return mDesc;
	}



	D3D11_USAGE NgaDx11TranslateUsage(NGABufferUsage usage)
	{
		if (usage & NGA_BUFFER_USAGE_CPU_READ_WRITE) {
			return D3D11_USAGE_STAGING;
		}

		if (usage & NGA_BUFFER_USAGE_GPU_WRITE) {
			return D3D11_USAGE_DEFAULT;
		}

		if (usage & NGA_BUFFER_USAGE_CPU_WRITE) {
			return D3D11_USAGE_DYNAMIC;
		}

		return D3D11_USAGE_IMMUTABLE;
	}

	UINT NgaDx11TranslateBindUsage(NGABufferUsage usage)
	{
		UINT dxusage = 0;

		if (usage & NGA_BUFFER_USAGE_VERTEX) {
			dxusage |= D3D11_BIND_VERTEX_BUFFER;
		}
		if (usage & NGA_BUFFER_USAGE_INDEX) {
			dxusage |= D3D11_BIND_INDEX_BUFFER;
		}
		if (usage & NGA_BUFFER_USAGE_CONSTANT) {
			CORE_ASSERT(dxusage == 0, "Constant buffers cannot be combined with any other binding type.");
			dxusage = D3D11_BIND_CONSTANT_BUFFER;
		}

		return dxusage;
	}

	UINT NgaDx11TranslateCPUAccessFlags(NGABufferUsage usage)
	{
		if (usage & NGA_BUFFER_USAGE_CPU_WRITE) {
			return D3D11_CPU_ACCESS_WRITE;
		}

		if (usage & NGA_BUFFER_USAGE_CPU_READ_WRITE) {
			return D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		}

		return 0;
	}

	bool NGABuffer::Construct(const NGABufferDesc &desc)
	{
		return Construct(desc, nullptr);
	}

	bool NGABuffer::Construct(const NGABufferDesc &desc, void *initialData)
	{
		CORE_ASSERT_RETURN_VALUE(!IsConstructed(), false, "Buffer is already constructed.");

		D3D11_BUFFER_DESC dxDesc;
		dxDesc.Usage = NgaDx11TranslateUsage(desc.mUsage);
		dxDesc.BindFlags = NgaDx11TranslateBindUsage(desc.mUsage);
		dxDesc.ByteWidth = (UINT)desc.mSizeInBytes;
		dxDesc.CPUAccessFlags = NgaDx11TranslateCPUAccessFlags(desc.mUsage);
		dxDesc.MiscFlags = 0;
		dxDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA *pSubResData = nullptr;
		D3D11_SUBRESOURCE_DATA subResData;
		if (initialData != nullptr) {
			subResData.pSysMem = initialData;
			pSubResData = &subResData;
		}

		HRESULT hr = NgaDx11State.mDevice->CreateBuffer(&dxDesc, pSubResData, &mBuffer);
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "ID3D11Device::CreateBuffer() failed with HRESULT %X", hr);

		mDesc = desc;

		return true;
	}

	void NGABuffer::Destruct()
	{
		COM_SAFE_RELEASE(mBuffer);
	}

	bool NGABuffer::IsConstructed()const
	{
		return mBuffer != nullptr;
	}
}

#endif // CORE_RENDER_API(DX11)