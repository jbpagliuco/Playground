#pragma once

#include <string>

namespace playground
{
	class DynamicMaterialInstance;
	class Material;

	class Texture;
	struct Vector4f;

	class MaterialContainer
	{
	public:
		bool Initialize(Material* material);
		void Shutdown();

		void Bind();

		void CreateDynamicMaterialInstance();

		bool SetFloatParameter(const std::string &name, float value);
		bool SetVectorParameter(const std::string& name, const Vector4f& value);
		bool SetTextureParameter(const std::string &name, const Texture *pTexture);

		Material* GetMaterial()const;

	private:
		Material *mMaterial;
		DynamicMaterialInstance *mDynMaterialInst;
	};
}