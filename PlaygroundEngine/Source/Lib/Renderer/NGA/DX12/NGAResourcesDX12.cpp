#include "NGA/NGAResources.h"

#if CORE_RENDER_API(DX12)

#include "DirectXTK12/Include/DDSTextureLoader.h"

#include "Core/Util/String.h"

#include "NGACoreInternalDX12.h"

namespace playground
{
	static D3D12_RESOURCE_FLAGS CreateBindFlags(const NGATextureDesc& desc)
	{
		D3D12_RESOURCE_FLAGS bindFlags = D3D12_RESOURCE_FLAG_NONE;

		bindFlags |= (desc.mBindFlags & NGA_TEXTURE_BIND_RENDER_TARGET) ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
		bindFlags |= (desc.mBindFlags & NGA_TEXTURE_BIND_DEPTH_STENCIL) ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;

		return bindFlags;
	}

	static D3D12_RESOURCE_STATES GetResourceState(const NGATextureDesc& desc)
	{
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		state |= (desc.mBindFlags & NGA_TEXTURE_BIND_RENDER_TARGET) ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_COMMON;
		state |= (desc.mBindFlags & NGA_TEXTURE_BIND_DEPTH_STENCIL) ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_COMMON;

		return state;
	}

	bool NGATexture::Construct(const NGATextureDesc& desc, void* initialData)
	{
		mDesc = desc;

		CORE_ASSERT_RETURN_VALUE(desc.mWidth > 0 && desc.mHeight > 0, false, "Invalid texture dimension: (%d, %d).", desc.mWidth, desc.mHeight);
		CORE_ASSERT_RETURN_VALUE(desc.mBindFlags != NGA_TEXTURE_BIND_NONE, false, "Texture MUST be bound.");

		if (desc.mUsage == NGAUsage::IMMUTABLE) {
			CORE_ASSERT_RETURN_VALUE(initialData != nullptr, false, "Immutable textures MUST be provided with texture data.");
		}

		if (desc.mType != NGATextureType::TEXTURE2D) {
			CORE_UNIMPLEMENTED();
			return false;
		}

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Width = desc.mWidth;
		resourceDesc.Height = desc.mHeight;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = NGAFormatToDXGI(desc.mFormat);
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = CreateBindFlags(desc);

		D3D12_CLEAR_VALUE clearValue;
		desc.mClearValue.mColor.ToFloatArray(clearValue.Color);
		clearValue.DepthStencil.Depth = desc.mClearValue.mDepth;
		clearValue.DepthStencil.Stencil = desc.mClearValue.mStencil;
		clearValue.Format = resourceDesc.Format;

		const D3D12_CLEAR_VALUE* clearValuePtr = desc.mClear ? &clearValue : nullptr;
		HRESULT hr = NgaDx12State.mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, GetResourceState(desc), clearValuePtr, IID_PPV_ARGS(&mResource));
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create constant buffer resource.");
		
		return true;
	}

	bool NGATexture::ConstructFromFile(const NGATextureDesc& desc, const char* filename)
	{
		CORE_UNIMPLEMENTED();
		return false;
	}

	void NGATexture::Destruct()
	{
		COM_SAFE_RELEASE(mResource);
	}

	bool NGATexture::IsConstructed()const
	{
		return mResource != nullptr;
	}




	bool NGABuffer::CreateBuffer(const NGABufferDesc& desc)
	{
		// Create the actual buffer resource.
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC bufferDesc{};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = desc.mSizeInBytes;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = NgaDx12State.mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&mBuffer));
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create constant buffer resource.");

		return true;
	}

	bool NGABuffer::CreateUploadBuffer(const NGABufferDesc& desc)
	{
		// In order to copy CPU memory data into our default buffer, we need to create an intermediate upload heap.
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProperties.CreationNodeMask = 1;
		uploadHeapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC bufferDesc{};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = desc.mSizeInBytes;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = NgaDx12State.mDevice->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploadBuffer));
		CORE_ASSERT_RETURN_VALUE(SUCCEEDED(hr), false, "Failed to create constant buffer upload resource.");

		return true;
	}

	bool NGABuffer::Construct(const NGABufferDesc& desc)
	{
		mDesc = desc;

		if (!CreateBuffer(desc)) {
			Destruct();
			return false;
		}

		if (!CreateView(desc)) {
			Destruct();
			return false;
		}

		if (!CreateUploadBuffer(desc)) {
			Destruct();
			return false;
		}

		return true;
	}

	bool NGABuffer::Construct(const NGABufferDesc& desc, void* initialData)
	{
		if (!Construct(desc)) {
			Destruct();
			return false;
		}

		if (initialData != nullptr) {
			// Describe the data we want to copy into the default buffer.
			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = initialData;
			subResourceData.RowPitch = desc.mSizeInBytes;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			// Schedule to copy the data to the default buffer resource. At a high level, the helper function UpdateSubresources
			// will copy the CPU memory into the intermediate upload heap. Then, using ID3D12CommandList::CopySubresourceRegion,
			// the intermediate upload heap data will be copied to mBuffer.
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = mBuffer;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);

			UpdateSubresources<1>(NgaDx12State.mCommandList, mBuffer, mUploadBuffer, 0, 0, 1, &subResourceData);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
			NgaDx12State.mCommandList->ResourceBarrier(1, &barrier);
		}

		return true;
	}

	bool NGABuffer::CreateView(const NGABufferDesc& desc)
	{
		if (IsVertexBuffer()) {
			CORE_ASSERT(desc.mVertexStride > 0, "Vertex stride cannot be zero.");

			mVertexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
			mVertexBufferView.StrideInBytes = desc.mVertexStride;
			mVertexBufferView.SizeInBytes = desc.mSizeInBytes;
			
			return true;
		}
		else if (IsIndexBuffer()) {
			CORE_ASSERT(desc.mIndexFormat == NGAFormat::R16_UINT || desc.mIndexFormat == NGAFormat::R32_UINT, "Invalid index format.");

			mIndexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
			mIndexBufferView.Format = NGAFormatToDXGI(desc.mIndexFormat);
			mIndexBufferView.SizeInBytes = desc.mSizeInBytes;

			return true;
		}
		else if (IsConstantBuffer()) {
			mConstantBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
			mConstantBufferView.SizeInBytes = desc.mSizeInBytes;

			return true;
		}

		CORE_UNIMPLEMENTED("Unhandled buffer type.");
		return false;
	}

	void NGABuffer::Destruct()
	{
		COM_SAFE_RELEASE(mBuffer);
		COM_SAFE_RELEASE(mUploadBuffer);
	}

	bool NGABuffer::IsConstructed()const
	{
		return mBuffer != nullptr;
	}
}

#endif // CORE_RENDER_API(DX11)