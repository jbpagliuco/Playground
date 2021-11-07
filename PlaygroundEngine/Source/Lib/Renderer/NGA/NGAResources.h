#pragma once

#include "NGA/NGACommon.h"

#include "Core/Util/BitUtil.h"
#include "Core/Util/Color.h"

#if CORE_RENDER_API(DX12)
#include "NGA/DX12/NGADescriptorHeapDX12.h"
#endif

struct ID3D12Resource;

namespace playground
{
	/////////////////////////////////////////////////////
	// Textures
	enum class REFLECTED NGATextureType : uint8_t
	{
		TEXTURE1D,
		TEXTURE2D,
		TEXTURE3D,
		TEXTURECUBE,
		UNKNOWN
	};

	struct NGAClearValue
	{
		ColorF mColor;
		float mDepth;
		uint8_t mStencil;
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

		bool mClear = false;
		NGAClearValue mClearValue;
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
		struct ID3D11Resource* mResource = nullptr;
#elif CORE_RENDER_API(DX12)
	private:
		ID3D12Resource* mResource = nullptr;
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
		// General buffer info
		NGABufferUsage mUsage = NGA_BUFFER_USAGE_NONE;
		uint32_t mSizeInBytes = 0;

		// Vertex buffer info
		size_t mVertexStride = 0;

		// Index buffer info
		NGAFormat mIndexFormat = NGAFormat::R16_UINT;
	};

	class NGABuffer
	{
		NGA_GPU_CLASS(NGABuffer);

	public:
		bool Construct(const NGABufferDesc &desc);
		bool Construct(const NGABufferDesc &desc, void *initialData);
		void Destruct();

		bool IsConstructed()const;

		size_t GetBufferSize()const;

		// Buffer type
		bool IsVertexBuffer()const;
		bool IsIndexBuffer()const;
		bool IsConstantBuffer()const;

		// Read/Write access
		bool IsCPUWritable()const;

		// Vertex buffers
		size_t GetVertexStride()const;
		NGAFormat GetIndexBufferFormat()const;

	private:
		NGABufferDesc mDesc;

#if CORE_RENDER_API(DX11)
	private:
		struct ID3D11Buffer* mBuffer = nullptr;
#elif CORE_RENDER_API(DX12)
	private:
		bool CreateBuffer(const NGABufferDesc &desc);
		bool CreateUploadBuffer(const NGABufferDesc &desc);
		bool CreateView(const NGABufferDesc &desc);

		ID3D12Resource* mBuffer = nullptr;

		// TODO: Ideally these shouldn't be stored here. These two fields are used for uploading the
		// initial data to the constant buffer. We must keep references to these since the upload is deferred.
		ID3D12Resource* mIntermediateUploadBuffer = nullptr;
		ID3DBlob* mIntermediateBlobCPU = nullptr;

		// View to this buffer.
		union {
			D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
			D3D12_CONSTANT_BUFFER_VIEW_DESC mConstantBufferView;
		};
#endif

		friend class NGACommandContext;
	};
}