#pragma once

#include "Core/Streaming/AssetFactory.h"

#include "Renderer/NGA/NGACommon.h"
#include "Resources/IndexBuffer.h"
#include "Resources/VertexBuffer.h"

namespace playground
{
	struct MeshData
	{
		void *vertices;
		size_t numVertices;
		size_t vertexStride;

		IndexType *indices;
		size_t numIndices;

		NGAVertexFormatDesc mVertexFormat;
	};

	class Mesh : public AssetFactory<Mesh>
	{
	public:
		bool Initialize(const MeshData &meshData);
		void Shutdown();

		void Render();

		inline const NGAVertexFormatDesc& GetVertexFormatDesc()const { return mVertexFormat; }

	private:
		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;

		NGAVertexFormatDesc mVertexFormat;
	};
}