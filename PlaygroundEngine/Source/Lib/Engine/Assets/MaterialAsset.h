#pragma once

#include "Core/Reflection/ReflMarkup.h"
#include "Core/Streaming/Stream.h"

#include "Renderer/Material/DynamicMaterial.h"
#include "Renderer/Material/MaterialContainer.h"
#include "Renderer/Material/StaticMaterial.h"

#include "MaterialAsset.reflgen.h"

namespace playground
{
	class Material;
	class RenderTarget;
	class TextureAsset;

	enum class REFLECTED MaterialParameterType : uint8_t
	{
		FLOAT,
		COLOR,
		VECTOR,
		TEXTURE,
		RENDER_TARGET,
		RENDER_TARGET_DEPTH,
	};

	struct REFLECTED MaterialParameter
	{
		GENERATED_REFLECTION_CODE();

	public:
		std::string mName				REFLECTED;
		MaterialParameterType mType		REFLECTED;

		float mFloat		REFL_ENUM_MATCH("mValue", mType, FLOAT);
		ColorF mColor		REFL_ENUM_MATCH("mValue", mType, COLOR);
		Vector4f mVector	REFL_ENUM_MATCH("mValue", mType, VECTOR);
		AssetID mAssetId	REFL_ENUM_MATCH("mValue", mType, TEXTURE, RENDER_TARGET, RENDER_TARGET_DEPTH) = INVALID_ASSET_ID;
	};

	struct REFLECTED MaterialAssetDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		AssetID mShaderID								REFLECTED REFL_NAME("shader");
		std::vector<MaterialParameter> mParameters		REFLECTED;
		bool mIsDynamic									REFLECTED = false;
	};

	class MaterialAsset
	{
	public:
		virtual bool Initialize(const MaterialAssetDesc& materialDesc);
		void Shutdown();

		virtual AssetID GetMaterialAssetID() = 0;

		Shader& GetShader()const;
		virtual Material& GetMaterial() = 0;

		MaterialContainer& GetMaterialContainer();

		bool SetFloatParameter(const std::string& name, float value);
		bool SetVectorParameter(const std::string& name, const Vector4f& value);

		bool SetTextureParameter(const std::string& name, const std::string& filename);
		bool SetTextureParameter(const std::string& name, AssetID textureId);
		bool SetTextureParameter(const std::string& name, TextureAsset* pTexture);

		bool SetRenderTargetParameter(const std::string& name, const std::string& filename, bool useColorMap);
		bool SetRenderTargetParameter(const std::string& name, AssetID renderTargetId, bool useColorMap);
		bool SetRenderTargetParameter(const std::string& name, RenderTarget* renderTarget, bool useColorMap);

	private:
		void ReleaseAssetByKey(const std::string& parameterName);
		void ReleaseAssets();

	protected:
		struct ProcessedParameters
		{
			// Holds constant buffer data. Must be freed.
			void* mConstantBuffer;

			// Size of the constant buffer data.
			size_t mConstantBufferSize;

			// List of all textures referenced in the parameters.
			std::vector<const Texture*> mTextures;

			// Parameter map for dynamic materials.
			DynamicMaterialParameterMap mDynamicMaterialParameterMap;

			// Current texture index for the dynamic parameter map.
			int mCurrentTextureIndex = 0;
		};

		ProcessedParameters ProcessParameters(const std::vector<MaterialParameter>& parameters);

	protected:
		AssetID mShaderID;
		std::vector<AssetID> mStaticAssets;
		std::map<std::string, AssetID> mDynamicAssets;

		MaterialContainer mMaterialContainer;
	};

	class StaticMaterialAsset : public MaterialAsset, public AssetFactory<StaticMaterialAsset>
	{
	public:
		virtual bool Initialize(const MaterialAssetDesc& materialDesc) override;
		void Shutdown();

		virtual AssetID GetMaterialAssetID() override;

		virtual Material& GetMaterial() override;

	private:
		StaticMaterial mMaterial;
	};

	class DynamicMaterialAsset : public MaterialAsset, public AssetFactory<DynamicMaterialAsset>
	{
	public:
		virtual bool Initialize(const MaterialAssetDesc& materialDesc) override; 
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