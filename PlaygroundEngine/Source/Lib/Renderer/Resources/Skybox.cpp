#include "Skybox.h"

namespace playground
{
	CORE_FACTORY_SETUP(Skybox);

	bool Skybox::Initialize(const SkyboxDesc& skyboxDesc)
	{
		TextureDesc desc;
		desc.mTextureDesc.mBindFlags = NGA_TEXTURE_BIND_SHADER_RESOURCE;
		desc.mTextureDesc.mType = NGATextureType::TEXTURECUBE;

		bool success = mSkyboxTexture.Initialize(desc, skyboxDesc.mFilename, true);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create skybox texture %s", skyboxDesc.mFilename.c_str());

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