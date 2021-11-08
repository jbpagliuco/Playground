#pragma once

#include "RenderableInstance.h"

#include "Core/Streaming/Stream.h"

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

		// Gets this mesh instance's material.
		virtual Material* GetMaterial()const override;

	private:
		Mesh *mMesh;
		MaterialContainer *mMaterialContainer;
	};
}