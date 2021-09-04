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

		Material* material = mMaterialContainer->GetMaterial();

		const VertexShader &vs = material->GetShader()->GetVertexShader();

		// Build the input layout
		NGAVertexFormatDesc vDesc;
		for (auto &shaderInput : vs.GetVertexFormatDesc().mAttributes) {
			NGAVertexAttribute attr;
			
			// Find the matching input from the mesh.
			bool found = false;
			for (auto &meshAttr : mMesh->GetVertexFormatDesc().mAttributes) {
				if (shaderInput.mSemanticType == meshAttr.mSemanticType && shaderInput.mSemanticIndex == meshAttr.mSemanticIndex) {
					attr = shaderInput;
					attr.mOffset = meshAttr.mOffset;
					found = true;
					break;
				}
			}

			CORE_ASSERT_RETURN_VALUE(found, false,
				"Shader '%s' requires %s%d, but mesh '%s' does not supply it.",
				GetAssetFilename(material->GetShader()->GetID()),
				GetSemanticName(shaderInput.mSemanticType),
				shaderInput.mSemanticIndex,
				GetAssetFilename(mMesh->GetID()));

			vDesc.mAttributes.push_back(attr);
		}

		mInputLayout.Construct(vDesc, vs.GetShader());

		return true;
	}
	
	void MeshInstance::Shutdown()
	{
		mMesh = nullptr;
		mMaterialContainer = nullptr;

		mInputLayout.Destruct();
	}

	void MeshInstance::Render(bool bindMaterial)
	{
		if (bindMaterial) {
			mMaterialContainer->Bind();
		}

		Playground_RendererStateManager->BindInputLayout(mInputLayout);
		mMesh->Render();
	}
}