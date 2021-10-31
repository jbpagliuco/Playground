#include "Material.h"

#include "StaticMaterial.h"
#include "DynamicMaterial.h"

namespace playground
{
	bool Material::Initialize(Shader *shader, const std::string& name)
	{
		mShader = shader;

#if RENDER_DEBUG_FEATURE(STORE_NAMES)
		mDebugName = name;
#endif

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