#include "MeshInstance.h"

#include "Renderer/Material/Material.h"
#include "Renderer/Material/DynamicMaterial.h"
#include "Renderer/Material/MaterialContainer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Resources/IndexBuffer.h"
#include "Renderer/Resources/VertexBuffer.h"

namespace playground
{
	bool MeshInstance::Initialize(Mesh *mesh, MaterialContainer *materialContainer)
	{
		mMesh = mesh;
		mMaterialContainer = materialContainer;

		return true;
	}
	
	void MeshInstance::Shutdown()
	{
		mMesh = nullptr;
		mMaterialContainer = nullptr;
	}

	void MeshInstance::Render(bool bindMaterial)
	{
		if (bindMaterial) {
			mMaterialContainer->Bind();
		}

		mMesh->Render();
	}

	Material* MeshInstance::GetMaterial()const
	{
		return mMaterialContainer->GetMaterial();
	}
}