#pragma once

#include "Material.h"

#include "Core/Util/Color.h"
#include "Core/Util/Serialize.h"
#include "Renderer/Resources/ConstantBuffer.h"

namespace playground
{
	class DynamicMaterialInstance;
	class RenderTarget;
	class Texture;

	struct DynamicMaterialParameterInfo
	{
		size_t mOffset;
		size_t mLength;
	};

	class DynamicMaterial : public Material
	{
	public:
		virtual bool Initialize(Shader *shader, size_t parameterByteLength, const DeserializationParameterMap &map, void *defaultParameterData, const std::vector<const Texture*> &defaultTextures);
		virtual void Shutdown() override;

		virtual void Bind() override;

		virtual int GetMaterialType()const override { return MATERIAL_TYPE_DYNAMIC; }

	protected:
		bool GetParameterInfo(DynamicMaterialParameterInfo &info, const std::string &name);
		int GetTextureParameterIndex(const std::string &name);

		void* GetDefaultParameterData()const;
		std::vector<const Texture*> GetDefaultTextures()const;

		ConstantBuffer mConstantBuffer;

	private:
		std::map<std::string, DynamicMaterialParameterInfo> mParameterMap;
		std::map<std::string, int> mTextureParameterMap;

		void *mDefaultParameterData;
		std::vector<const Texture*> mDefaultTextures;

		friend class DynamicMaterialInstance;
	};



	class DynamicMaterialInstance
	{
	public:
		DynamicMaterialInstance(DynamicMaterial *pMaterial);

		void Shutdown();

		void BindDynamicData();

		void SetFloatParameter(const std::string &name, float value);
		void SetVectorParameter(const std::string& name, const Vector4f& value);
		void SetTextureParameter(const std::string &name, const Texture *pTexture);
		
	private:
		DynamicMaterial *mParent;

		void *mParameterData;
		std::vector<const Texture*> mTextures;
	};

	DynamicMaterialInstance* CreateDynamicMaterialInstance(DynamicMaterial *pParent);
	void DestroyDynamicMaterialInstance(DynamicMaterialInstance *pInst);
}