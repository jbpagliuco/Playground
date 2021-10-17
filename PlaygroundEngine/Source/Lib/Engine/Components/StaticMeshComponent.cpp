#include "StaticMeshComponent.h"

#include "Core/Streaming/Stream.h"
#include "Core/Reflection/ReflSerialize.h"

#include "Renderer/Material/Material.h"
#include "Renderer/Material/DynamicMaterial.h"
#include "Renderer/Mesh.h"
#include "Renderer/Scene/Scene.h"

#include "Engine/World/GameObject.h"
#include "Engine/Assets/MaterialAsset.h"

namespace playground
{
	void StaticMeshComponent::DeserializePost(const DeserializationParameterMap& params)
	{
		mMaterialAsset = GetMaterialByAssetID(mMaterialID);
		
		Mesh* mesh = Mesh::Get(mMeshID);
		MaterialContainer& materialContainer = mMaterialAsset->GetMaterialContainer();

		mMeshInstance.Initialize(mesh, &materialContainer);

		// Set overrides
		auto &materialParams = params["material"];
		if (materialParams.HasChild("overrides")) {
			// Create the dynamic instance
			materialContainer.CreateDynamicMaterialInstance();

			DynamicMaterialAsset* dynamicMaterial = static_cast<DynamicMaterialAsset*>(mMaterialAsset);

			for (auto& overrideParam : materialParams["overrides"].childrenArray) {
				MaterialParameterOverride materialOverride;
				ReflectionDeserialize(MaterialParameterOverride::StaticClass(), &materialOverride, overrideParam);

				switch (materialOverride.mType) {
				case MaterialParameterType::FLOAT:
					dynamicMaterial->SetFloatParameter(materialOverride.mName, materialOverride.mFloat);
					break;

				case MaterialParameterType::VECTOR:
					dynamicMaterial->SetVectorParameter(materialOverride.mName, materialOverride.mVector);
					break;

				case MaterialParameterType::TEXTURE:
				{
					dynamicMaterial->SetTextureParameter(materialOverride.mName, materialOverride.mAssetId);

					// Release our reference to this asset immediately. The material will still hold a reference.
					ReleaseAsset(materialOverride.mAssetId);
					break;
				}

				case MaterialParameterType::RENDER_TARGET:
				case MaterialParameterType::RENDER_TARGET_DEPTH:
				{
					const bool useColorMap = materialOverride.mType == MaterialParameterType::RENDER_TARGET;
					dynamicMaterial->SetRenderTargetParameter(materialOverride.mName, materialOverride.mAssetId, useColorMap);

					// Release our reference to this asset immediately. The material will still hold a reference.
					ReleaseAsset(materialOverride.mAssetId);
					break;
				}
				};
			}
		}
	}

	void StaticMeshComponent::Activate()
	{
		Scene::Get()->AddRenderable(&mMeshInstance);
	}

	void StaticMeshComponent::Deactivate()
	{
		Scene::Get()->RemoveRenderable(&mMeshInstance);

		mMeshInstance.Shutdown();

		ReleaseAsset(mMaterialAsset->GetMaterialAssetID());
		ReleaseAsset(mMeshID);
	}

	void StaticMeshComponent::UpdateLate(float deltaTime)
	{
		mMeshInstance.SetWorldTransform(GetOwner()->mTransform.GetMatrix());
	}
}