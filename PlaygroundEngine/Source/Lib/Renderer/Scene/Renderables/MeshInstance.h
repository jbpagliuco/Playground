#pragma once

#include "RenderableInstance.h"

#include "Core/Streaming/Stream.h"
#include "Renderer/NGA/NGAInputLayout.h"

namespace playground
{
	class MaterialContainer;
	class Mesh;

	class MeshInstance : public RenderableInstance
	{
	public:
		bool Initialize(Mesh *mesh, MaterialContainer *materialContainer);
		void Shutdown();

		virtual void Render(bool bindMaterial = true) override;

	private:
		Mesh *mMesh;
		MaterialContainer *mMaterialContainer;

		NGAInputLayout mInputLayout;
	};
}