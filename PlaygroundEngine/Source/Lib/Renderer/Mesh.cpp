#include "Mesh.h"

#include "Renderer.h"

namespace playground
{
	CORE_FACTORY_SETUP(Mesh);

	bool Mesh::Initialize(const MeshData &meshData)
	{
#if RENDER_DEBUG_FEATURE(STORE_NAMES)
		mDebugName = meshData.mName;
#endif

		COMMAND_LIST_SCOPE();

		if (!mVertexBuffer.Initialize(meshData.vertices, meshData.numVertices, meshData.vertexStride)) {
			return false;
		}

		if (!mIndexBuffer.Initialize(meshData.indices, meshData.numIndices)) {
			mVertexBuffer.Shutdown();
			return false;
		}

		mVertexFormat = meshData.mVertexFormat;

		return true;
	}

	void Mesh::Shutdown()
	{
		mVertexBuffer.Shutdown();
		mIndexBuffer.Shutdown();
	}

	void Mesh::Render()
	{
		Playground_RendererStateManager->SetPrimitiveTopology(NGAPrimitiveTopology::TRIANGLE_LIST);
		Playground_RendererStateManager->BindIndexBuffer(mIndexBuffer);
		Playground_RendererStateManager->BindVertexBuffer(mVertexBuffer);
		Playground_RendererStateManager->DrawIndexed(mIndexBuffer);
	}
}