#include "TextureAsset.h"

#include "Core/Streaming/Stream.h"
#include "Core/Reflection/ReflSerialize.h"
#include "Renderer/Resources/RenderTarget.h"
#include "Renderer/Resources/Skybox.h"
#include "Renderer/Resources/Texture.h"

namespace playground
{
	CORE_FACTORY_SETUP(TextureAsset);


	static bool OnTextureTexxLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
	static bool OnTextureDDSLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
	static void OnTextureUnload(const AssetID &id);

	static bool OnRenderTargetLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header, const DeserializationParameterMap& parameters);
	static void OnRenderTargetUnload(const AssetID &id);

	static bool OnSkyboxLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header, const DeserializationParameterMap& parameters);
	static void OnSkyboxUnload(const AssetID &id);




	bool TextureAsset::Initialize(const std::string& filename)
	{
		TextureDesc desc;
		desc.mTextureDesc.mBindFlags = NGA_TEXTURE_BIND_SHADER_RESOURCE;
		desc.mTextureDesc.mUsage = NGAUsage::IMMUTABLE;
		desc.mSamplerStateDesc = NGASamplerStateDesc();

		bool success = mTexture.Initialize(desc, filename, true);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to create texture %s", filename.c_str());

		return true;
	}

	void TextureAsset::Shutdown()
	{
		mTexture.Shutdown();
	}

	const Texture& TextureAsset::GetTexture()const
	{
		return mTexture;
	}





	bool TextureAssetSystemInit()
	{
		AssetType texType;
		texType.mExt = "texx";
		texType.mOnLoadRaw = OnTextureTexxLoad;
		texType.mOnUnload = OnTextureUnload;
		RegisterAssetType(texType);

		AssetType ddsType;
		ddsType.mExt = "dds";
		ddsType.mOnLoadRaw = OnTextureDDSLoad;
		ddsType.mOnUnload = OnTextureUnload;
		RegisterAssetType(ddsType);

		AssetType rtxType;
		rtxType.mExt = "rtx";
		rtxType.mOnLoadXML = OnRenderTargetLoad;
		rtxType.mOnUnload = OnRenderTargetUnload;
		RegisterAssetType(rtxType);

		AssetType skyxType;
		skyxType.mExt = "skyx";
		skyxType.mOnLoadXML = OnSkyboxLoad;
		skyxType.mOnUnload = OnSkyboxUnload;
		RegisterAssetType(skyxType);

		return true;
	}

	void TextureAssetSystemShutdown()
	{
		CORE_ASSERT(TextureAsset::ReportEmpty(), "There were still textures allocated during shutdown!");
		TextureAsset::ReleaseAll();

		CORE_ASSERT(RenderTarget::ReportEmpty(), "There were still render targets allocated during shutdown!");
		RenderTarget::ReleaseAll();

		CORE_ASSERT(Skybox::ReportEmpty(), "There were still skyboxes allocated during shutdown!");
		Skybox::ReleaseAll();
	}


	static bool OnTextureTexxLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header)
	{
		/*Texture *pTex = Texture::Create(id);
		CORE_ASSERT_RETURN_VALUE(pTex != nullptr, false, "Failed to allocate texture");

		DeserializationParameterMap params = ParseFile(filename);

		const std::string textureFile = params["textureFile"].AsFilepath();

		static std::map<std::string, TextureFilter> _filterConv = {
			{ "point", TextureFilter::POINT },
			{ "point_min_mag", TextureFilter::POINT_MIN_MAG },
			{ "point_min_mip", TextureFilter::POINT_MIN_MIP },
			{ "point_mag_mip", TextureFilter::POINT_MAG_MIP },
			{ "linear", TextureFilter::LINEAR },
			{ "linear_min_mag", TextureFilter::LINEAR_MIN_MAG },
			{ "linear_min_mip", TextureFilter::LINEAR_MIN_MIP },
			{ "linear_mag_mip", TextureFilter::LINEAR_MAG_MIP },
			{ "anisotropic", TextureFilter::ANISOTROPIC },
		};

		static std::map<std::string, TextureAddressMode> _addressModeConv = {
			{ "wrap", TextureAddressMode::WRAP },
			{ "mirror", TextureAddressMode::MIRROR },
			{ "clamp", TextureAddressMode::CLAMP },
			{ "border", TextureAddressMode::BORDER },
			{ "mirror_once", TextureAddressMode::MIRROR_ONCE },
		};

		DeserializationParameterMap samplerStateParamMap = params["sampler"];

		SamplerStateParameters samplerStateParams;
		samplerStateParams.mFilter = _filterConv[samplerStateParamMap["filter"].AsString("point")];
		samplerStateParams.mAddressU = _addressModeConv[samplerStateParamMap["addressU"].AsString("wrap")];
		samplerStateParams.mAddressV = _addressModeConv[samplerStateParamMap["addressV"].AsString("wrap")];
		samplerStateParams.mAddressW = _addressModeConv[samplerStateParamMap["addressW"].AsString("wrap")];
		samplerStateParams.mBorderColor = samplerStateParamMap.AsColor();
		
		return pTex->Initialize(textureFile, samplerStateParams);*/

		return false;
	}

	static bool OnTextureDDSLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header)
	{
		TextureAsset *pTex = TextureAsset::Create(id);
		CORE_ASSERT_RETURN_VALUE(pTex != nullptr, false, "Failed to allocate texture");

		// Create the texture right from a DDS file, with the normal sampler state parameters
		return pTex->Initialize(filename);
	}

	static void OnTextureUnload(const AssetID &id)
	{
		TextureAsset::Release(id);
	}




	static bool OnRenderTargetLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header, const DeserializationParameterMap& parameters)
	{
		RenderTarget *pRenderTarget = RenderTarget::Create(id);
		CORE_ASSERT_RETURN_VALUE(pRenderTarget != nullptr, false, "Failed to allocate render target");

		RenderTargetDesc desc;
		ReflectionDeserialize(RenderTargetDesc::StaticClass(), &desc, parameters);

		// Sanity check
		if (desc.mDepthMapDesc.mShaderResource) {
			CORE_ASSERT(desc.mDepthMapDesc.mFormat == NGAFormat::D32_FLOAT, "Depth map cannot use stencil map if it's a shader resource.");
			desc.mDepthMapDesc.mFormat = NGAFormat::R32_TYPELESS;
		}

		// Create the render target
		return pRenderTarget->Initialize(desc);
	}

	static void OnRenderTargetUnload(const AssetID &id)
	{
		RenderTarget::Release(id);
	}





	static bool OnSkyboxLoad(const AssetID& id, const std::string& filename, const AssetFileHeader& header, const DeserializationParameterMap& parameters)
	{
		Skybox* skybox = Skybox::Create(id);
		CORE_ASSERT_RETURN_VALUE(skybox != nullptr, false, "Failed to allocate skybox.");

		SkyboxDesc skyboxDesc;
		ReflectionDeserialize(SkyboxDesc::StaticClass(), &skyboxDesc, parameters);

		return skybox->Initialize(skyboxDesc);
	}

	static void OnSkyboxUnload(const AssetID &id)
	{
		Skybox::Release(id);
	}
}