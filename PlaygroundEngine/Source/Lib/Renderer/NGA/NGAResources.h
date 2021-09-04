#pragma once

#include "NGA/NGACommon.h"

#include "Core/Util/BitUtil.h"

namespace playground
{
	/////////////////////////////////////////////////////
	// Textures
	enum class NGATextureType
	{
		TEXTURE1D,
		TEXTURE2D,
		TEXTURE3D,
		TEXTURECUBE,
		UNKNOWN
	};

	struct NGATextureDesc
	{
		NGATextureType mType = NGATextureType::UNKNOWN;

		NGAUsage mUsage = NGAUsage::GPU_WRITE;
		NGAFormat mFormat = NGAFormat::UNKNOWN;

		NGATextureBindFlags mBindFlags = NGA_TEXTURE_BIND_NONE;

		int mWidth = 0;
		int mHeight = 0;

		int mArraySize = 1;
	};

	class NGATexture
	{
		NGA_GPU_CLASS(NGATexture);

	public:
		bool Construct(const NGATextureDesc &desc, void *initialData);
		bool ConstructFromFile(const NGATextureDesc &desc, const char *filename);
		void Destruct();

		bool IsConstructed()const;

		bool IsArray()const;

		const NGATextureDesc& GetDesc()const;
		
	private:
		NGATextureDesc mDesc;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11Resource *mResource;
#endif

		friend class NGAShaderResourceView;
		friend class NGARenderTargetView;
		friend class NGADepthStencilView;
	};


	/////////////////////////////////////////////////////
	// Buffers
	enum NGABufferUsage
	{
		NGA_BUFFER_USAGE_NONE			= 0,
		NGA_BUFFER_USAGE_VERTEX			= 1 << 0,
		NGA_BUFFER_USAGE_INDEX			= 1 << 1,
		NGA_BUFFER_USAGE_CONSTANT		= 1 << 2,

		// Immutable is the default if none of these flags are specified
		NGA_BUFFER_USAGE_GPU_WRITE		= 1 << 3,
		NGA_BUFFER_USAGE_CPU_WRITE		= 1 << 4,
		NGA_BUFFER_USAGE_CPU_READ_WRITE = 1 << 5
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(NGABufferUsage);

	struct NGABufferDesc
	{
		NGABufferUsage mUsage = NGA_BUFFER_USAGE_NONE;
		uint32_t mSizeInBytes = 0;
	};

	class NGABuffer
	{
		NGA_GPU_CLASS(NGABuffer);

	public:
		bool Construct(const NGABufferDesc &desc);
		bool Construct(const NGABufferDesc &desc, void *initialData);
		void Destruct();

		bool IsConstructed()const;

	private:
		NGABufferDesc mDesc;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11Buffer *mBuffer;

		friend class NGACommandContext;
#endif
	};
}