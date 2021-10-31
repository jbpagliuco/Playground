#pragma once

#include <vector>

#include "Renderer/Resources/ConstantBuffer.h"
#include "Material.h"

namespace playground
{
	class Texture;

	struct StaticMaterialDesc
	{
		std::string mName;

		Shader* mShader;

		void* mParameterData;
		size_t mParameterDataByteLength;

		std::vector<const Texture*> mTextures;
	};

	class StaticMaterial : public Material
	{
	public:
		bool Initialize(const StaticMaterialDesc& desc);
		virtual void Shutdown() override;

		virtual void Bind() override;

		virtual int GetMaterialType()const override { return MATERIAL_TYPE_STATIC; }

	private:
		ConstantBuffer mConstantBuffer;

		std::vector<const Texture*> mTextures;
	};
}