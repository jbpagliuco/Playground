#pragma once

#include "Core/Streaming/Stream.h"

#include "Renderer/Material/DynamicMaterial.h"
#include "Renderer/Material/MaterialContainer.h"
#include "Renderer/Material/StaticMaterial.h"

namespace playground
{
	class Material;
	class RenderTarget;
	class TextureAsset;

	class MaterialAsset
	{
	public:
		bool Initialize(const std::string &shaderName, const std::vector<AssetID> &assets);
		void Shutdown();

		virtual AssetID GetMaterialAssetID() = 0;

		Shader& GetShader()const;
		virtual Material& GetMaterial() = 0;

		MaterialContainer& GetMaterialContainer();

		bool SetFloatParameter(const std::string &name, float value);
		bool SetVectorParameter(const std::string& name, const Vector4f& value);

		bool SetTextureParameter(const std::string& name, const std::string& filename);
		bool SetTextureParameter(const std::string& name, AssetID textureId);
		bool SetTextureParameter(const std::string &name, TextureAsset *pTexture);

		bool SetRenderTargetParameter(const std::string& name, const std::string& filename, bool useColorMap);
		bool SetRenderTargetParameter(const std::string& name, AssetID renderTargetId, bool useColorMap);
		bool SetRenderTargetParameter(const std::string& name, RenderTarget *renderTarget, bool useColorMap);

	private:
		void ReleaseAssetByKey(const std::string& parameterName);
		void ReleaseAssets();

	protected:
		AssetID mShaderID;
		std::vector<AssetID> mStaticAssets;
		std::map<std::string, AssetID> mDynamicAssets;

		MaterialContainer mMaterialContainer;
	};

	class StaticMaterialAsset : public MaterialAsset, public AssetFactory<StaticMaterialAsset>
	{
	public:
		bool Initialize(const std::string &shaderName, const std::vector<AssetID> &assets, void *parameterData, size_t parameterByteLength, const std::vector<const Texture*> &textures);
		void Shutdown();

		virtual AssetID GetMaterialAssetID() override;

		virtual Material& GetMaterial() override;

	private:
		StaticMaterial mMaterial;
	};

	class DynamicMaterialAsset : public MaterialAsset, public AssetFactory<DynamicMaterialAsset>
	{
	public:
		bool Initialize(const std::string &shaderName, const std::vector<AssetID> &assets, DeserializationParameterMap parameterMap, void *parameterData, size_t parameterByteLength, const std::vector<const Texture*> &textures);
		void Shutdown();

		virtual AssetID GetMaterialAssetID() override;

		virtual Material& GetMaterial() override;

	private:
		DynamicMaterial mMaterial;
	};

	bool MaterialSystemInit();
	void MaterialSystemShutdown();

	MaterialAsset* GetMaterialByAssetID(AssetID id);
}