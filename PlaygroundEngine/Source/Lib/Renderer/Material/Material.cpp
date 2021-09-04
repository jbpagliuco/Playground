#include "Material.h"

#include "StaticMaterial.h"
#include "DynamicMaterial.h"

namespace playground
{
	bool Material::Initialize(Shader *shader)
	{
		mShader = shader;

		return true;
	}
	
	void Material::Shutdown()
	{
		mShader = nullptr;
	}

	void Material::Bind()
	{
		mShader->Bind();
	}
}