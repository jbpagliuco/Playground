#pragma once

#include <vector>

#include "Renderer/Resources/ConstantBuffer.h"
#include "Material.h"

namespace playground
{
	class Texture;

	class StaticMaterial : public Material
	{
	public:
		bool Initialize(Shader *shader, void *parameterData, size_t parameterByteLength, const std::vector<const Texture*> &textures);
		virtual void Shutdown() override;

		virtual void Bind() override;

		virtual int GetMaterialType()const override { return MATERIAL_TYPE_STATIC; }

	private:
		ConstantBuffer mConstantBuffer;

		std::vector<const Texture*> mTextures;
	};
}