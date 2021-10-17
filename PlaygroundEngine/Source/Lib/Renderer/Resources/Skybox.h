#pragma once

#include "Core/Reflection/ReflMarkup.h"
#include "Core/Streaming/AssetFactory.h"

#include "Renderer/Resources/Texture.h"

#include "Skybox.reflgen.h"

namespace playground
{
	struct REFLECTED SkyboxDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		std::string mFilename			REFLECTED REFL_FILEPATH;
	};

	class Skybox : public AssetFactory<Skybox>
	{
	public:
		bool Initialize(const SkyboxDesc& skyboxDesc);
		void Shutdown();

		const Texture& GetTexture()const;

	private:
		Texture mSkyboxTexture;
	};
}