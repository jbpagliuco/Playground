#include "MaterialContainer.h"

#include "Core/Debug/Assert.h"

#include "Renderer/Material/Material.h"
#include "Renderer/Material/DynamicMaterial.h"

namespace playground
{
	bool MaterialContainer::Initialize(Material* material)
	{
		mMaterial = material;

		return true;
	}

	void MaterialContainer::Shutdown()
	{
		if (mDynMaterialInst) {
			mDynMaterialInst->Shutdown();
			DestroyDynamicMaterialInstance(mDynMaterialInst);
			mDynMaterialInst = nullptr;
		}

		mMaterial = nullptr;
	}

	void MaterialContainer::Bind()
	{
		mMaterial->Bind();

		if (mDynMaterialInst) {
			mDynMaterialInst->BindDynamicData();
		}
	}

	void MaterialContainer::CreateDynamicMaterialInstance()
	{
		CORE_ASSERT_RETURN(mMaterial->GetMaterialType() == MATERIAL_TYPE_DYNAMIC);

		mDynMaterialInst = playground::CreateDynamicMaterialInstance(static_cast<DynamicMaterial*>(mMaterial));
		CORE_ASSERT(mDynMaterialInst != nullptr, "Failed to create dynamic material instance");
	}

	bool MaterialContainer::SetFloatParameter(const std::string& name, float value)
	{
		CORE_ASSERT_RETURN_VALUE(mDynMaterialInst != nullptr, false);
		mDynMaterialInst->SetFloatParameter(name, value);

		return true;
	}

	bool MaterialContainer::SetVectorParameter(const std::string& name, const Vector4f& value)
	{
		CORE_ASSERT_RETURN_VALUE(mDynMaterialInst != nullptr, false);
		mDynMaterialInst->SetVectorParameter(name, value);

		return true;
	}

	bool MaterialContainer::SetTextureParameter(const std::string& name, const Texture* texture)
	{
		CORE_ASSERT_RETURN_VALUE(mDynMaterialInst != nullptr, false);
		mDynMaterialInst->SetTextureParameter(name, texture);

		return true;
	}

	Material* MaterialContainer::GetMaterial()const
	{
		return mMaterial;
	}
}