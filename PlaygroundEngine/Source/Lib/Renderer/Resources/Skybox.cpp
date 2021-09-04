#include "Skybox.h"

namespace playground
{
	CORE_FACTORY_SETUP(Skybox);

	bool Skybox::Initialize(const std::string& filename)
	{
		TextureDesc desc;
		desc.mTextureDesc.mBindFlags = NGA_TEXTURE_BIND_SHADER_RESOURCE;
		desc.mTextureDesc.mType = NGATextureType::TEXTURECUBE;

		bool success = mSkyboxTexture.Initialize(desc, filename, true);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create skybox texture %s", filename);

		return true;
	}

	void Skybox::Shutdown()
	{
		mSkyboxTexture.Shutdown();
	}
	
	const Texture& Skybox::GetTexture()const
	{
		return mSkyboxTexture;
	}
}