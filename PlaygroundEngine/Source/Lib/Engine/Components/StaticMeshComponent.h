#pragma once

#include "Core/Reflection/ReflMarkup.h"
#include "Renderer/Scene/Renderables/MeshInstance.h"

#include "GameComponent.h"

#include "StaticMeshComponent.reflgen.h"

namespace playground
{
	class MaterialAsset;

	enum class REFLECTED MaterialParameterOverrideType : uint8_t
	{
		FLOAT,
		VECTOR,
		TEXTURE,
		RENDER_TARGET,
		RENDER_TARGET_DEPTH,
	};

	struct REFLECTED MaterialParameterOverride
	{
		GENERATED_REFLECTION_CODE();

	public:
		std::string mName						REFLECTED;
		MaterialParameterOverrideType mType		REFLECTED;

		float mFloat		REFL_ENUM_MATCH("mValue", mType, FLOAT);
		Vector4f mVector	REFL_ENUM_MATCH("mValue", mType, VECTOR);
		AssetID mAssetId	REFL_ENUM_MATCH("mValue", mType, TEXTURE, RENDER_TARGET, RENDER_TARGET_DEPTH) = INVALID_ASSET_ID;
	};

	class REFLECTED StaticMeshComponent : public GameComponentBase<GameComponentType::STATIC_MESH>
	{
		GENERATED_REFLECTION_CODE();

	public:
		virtual void DeserializePost(const DeserializationParameterMap& params) override;

		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	private:
		AssetID mMeshID			REFLECTED REFL_NAME("mesh") = INVALID_ASSET_ID;
		AssetID mMaterialID		REFLECTED REFL_NAME("material") = INVALID_ASSET_ID;

		MaterialAsset *mMaterialAsset;

		MeshInstance mMeshInstance;
	};
}