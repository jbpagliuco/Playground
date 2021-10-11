#pragma once

#include "Renderer/Scene/Renderables/MeshInstance.h"

#include "GameComponent.h"

#include "StaticMeshComponent.reflgen.h"

namespace playground
{
	class MaterialAsset;

	class REFLECTED StaticMeshComponent : public GameComponentBase<GameComponentType::STATIC_MESH>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void Deserialize(DeserializationParameterMap &params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	private:
		AssetID mMeshID;
		MaterialAsset *mMaterialAsset;

		MeshInstance mMeshInstance;
	};
}