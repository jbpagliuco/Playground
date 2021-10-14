#include "MaterialAsset.h"

#include "Core/Streaming/Stream.h"
#include "Core/File/File.h"
#include "Core/Util/Serialize.h"
#include "Renderer/Material/StaticMaterial.h"
#include "Renderer/Material/DynamicMaterial.h"
#include "Renderer/RenderingSystem.h"
#include "Renderer/Resources/RenderTarget.h"

#include "TextureAsset.h"

namespace playground
{
#define MATERIAL_ASSERT_CLEANUP(cond, rv, ...) CORE_ASSERT_RETURN_VALUE_CLEANUP(cond, rv, Shutdown, __VA_ARGS__)

	CORE_FACTORY_SETUP(StaticMaterialAsset);
	CORE_FACTORY_SETUP(DynamicMaterialAsset);

	static bool OnMaterialLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
	static void OnMaterialUnload(const AssetID &id);

	static bool OnShaderLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
	static void OnShaderUnload(const AssetID &id);




	bool MaterialAsset::Initialize(const std::string& shaderName, const std::vector<AssetID>& assets)
	{
		mShaderID = RequestAsset(shaderName);
		
		mStaticAssets = assets;

		return true;
	}

	void MaterialAsset::Shutdown()
	{
		ReleaseAssets();
		mMaterialContainer.Shutdown();
	}

	void MaterialAsset::ReleaseAssets()
	{
		for (auto& asset : mStaticAssets) {
			ReleaseAsset(asset);
		}

		for (auto& it : mDynamicAssets) {
			ReleaseAsset(it.second);
		}

		ReleaseAsset(mShaderID);
	}

	MaterialContainer& MaterialAsset::GetMaterialContainer()
	{
		return mMaterialContainer;
	}

	Shader& MaterialAsset::GetShader()const
	{
		return *Shader::Get(mShaderID);
	}

	bool MaterialAsset::SetFloatParameter(const std::string& name, float value)
	{
		return mMaterialContainer.SetFloatParameter(name, value);
	}

	bool MaterialAsset::SetVectorParameter(const std::string& name, const Vector4f& value)
	{
		return mMaterialContainer.SetVectorParameter(name, value);
	}

	bool MaterialAsset::SetTextureParameter(const std::string& name, const std::string &filename)
	{
		AssetID texID = RequestAsset(filename);
		TextureAsset* texture = TextureAsset::Get(texID);

		ReleaseAssetByKey(name);
		mDynamicAssets[name] = texID;

		return mMaterialContainer.SetTextureParameter(name, &texture->GetTexture());
	}

	bool MaterialAsset::SetTextureParameter(const std::string& name, TextureAsset* texture)
	{
		AddAssetRef(texture->GetID());

		ReleaseAssetByKey(name);
		mDynamicAssets[name] = texture->GetID();

		return mMaterialContainer.SetTextureParameter(name, &texture->GetTexture());
	}

	bool MaterialAsset::SetRenderTargetParameter(const std::string& name, const std::string& filename, bool useColorMap)
	{
		AssetID rtID = RequestAsset(filename);
		RenderTarget* renderTarget = RenderTarget::Get(rtID);

		ReleaseAssetByKey(name);
		mDynamicAssets[name] = rtID;

		if (useColorMap) {
			return mMaterialContainer.SetTextureParameter(name, &renderTarget->GetColorMap());
		}
		else {
			return mMaterialContainer.SetTextureParameter(name, &renderTarget->GetDepthMap());
		}
	}

	bool MaterialAsset::SetRenderTargetParameter(const std::string& name, RenderTarget* renderTarget, bool useColorMap)
	{
		AddAssetRef(renderTarget->GetID());

		ReleaseAssetByKey(name);
		mDynamicAssets[name] = renderTarget->GetID();

		if (useColorMap) {
			return mMaterialContainer.SetTextureParameter(name, &renderTarget->GetColorMap());
		}
		else {
			return mMaterialContainer.SetTextureParameter(name, &renderTarget->GetDepthMap());
		}
	}

	void MaterialAsset::ReleaseAssetByKey(const std::string& parameterName)
	{
		if (mDynamicAssets.find(parameterName) != mDynamicAssets.end()) {
			ReleaseAsset(mDynamicAssets[parameterName]);
			mDynamicAssets.erase(parameterName);
		}
	}


	bool StaticMaterialAsset::Initialize(const std::string& shaderName, const std::vector<AssetID>& assets, void* parameterData, size_t parameterByteLength, const std::vector<const Texture*>& textures)
	{
		if (!MaterialAsset::Initialize(shaderName, assets)) {
			return false;
		}

		bool success = mMaterial.Initialize(&GetShader(), parameterData, parameterByteLength, textures);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to create static material.");

		mMaterialContainer.Initialize(&mMaterial);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to material container.");

		return true;
	}

	void StaticMaterialAsset::Shutdown()
	{
		MaterialAsset::Shutdown();

		mMaterial.Shutdown();
	}

	AssetID StaticMaterialAsset::GetMaterialAssetID()
	{
		return GetID();
	}

	Material& StaticMaterialAsset::GetMaterial()
	{
		return mMaterial;
	}


	bool DynamicMaterialAsset::Initialize(const std::string& shaderName, const std::vector<AssetID>& assets, DeserializationParameterMap parameterMap, void* parameterData, size_t parameterByteLength, const std::vector<const Texture*>& textures)
	{
		if (!MaterialAsset::Initialize(shaderName, assets)) {
			return false;
		}

		bool success = mMaterial.Initialize(&GetShader(), parameterByteLength, parameterMap, parameterData, textures);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to create static material.");

		mMaterialContainer.Initialize(&mMaterial);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to material container.");

		return true;
	}

	void DynamicMaterialAsset::Shutdown()
	{
		MaterialAsset::Shutdown();

		mMaterial.Shutdown();
	}

	AssetID DynamicMaterialAsset::GetMaterialAssetID()
	{
		return GetID();
	}

	Material& DynamicMaterialAsset::GetMaterial()
	{
		return mMaterial;
	}






	bool MaterialSystemInit()
	{
		AssetType matType;
		matType.mExt = "matx";
		matType.mOnLoad = OnMaterialLoad;
		matType.mOnUnload = OnMaterialUnload;
		RegisterAssetType(matType);

		AssetType shaderType;
		shaderType.mExt = "shaderx";
		shaderType.mOnLoad = OnShaderLoad;
		shaderType.mOnUnload = OnShaderUnload;
		shaderType.mMinVersion = 2;
		shaderType.mMaxVersion = 2;
		RegisterAssetType(shaderType);

		return true;
	}

	void MaterialSystemShutdown()
	{
		CORE_ASSERT(StaticMaterialAsset::ReportEmpty(), "There were still materials allocated during shutdown!");
		StaticMaterialAsset::ReleaseAll();

		CORE_ASSERT(DynamicMaterialAsset::ReportEmpty(), "There were still materials allocated during shutdown!");
		DynamicMaterialAsset::ReleaseAll();

		CORE_ASSERT(Shader::ReportEmpty(), "There were still shaders allocated during shutdown!");
		Shader::ReleaseAll();
	}

	MaterialAsset* GetMaterialByAssetID(AssetID id)
	{
		if (StaticMaterialAsset::Exists(id)) {
			return StaticMaterialAsset::Get(id);
		}
		else if (DynamicMaterialAsset::Exists(id)) {
			return DynamicMaterialAsset::Get(id);
		}

		return nullptr;
	}



	static bool OnMaterialLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header)
	{
		DeserializationParameterMap params = ParseFile(filename);

		const std::string shaderFilename = params["shaderFile"].AsFilepath();
		const bool isDynamic = params["dynamic"].AsBool();

		// We will use this function as a common place to load in assets for the materials.
		// But it is the material's responsibility to release the assets.
		std::vector<AssetID> assets;
		std::vector<const Texture*> textures;

		// Read in parameters
		constexpr size_t MAX_MATERIAL_PARAMETER_BYTE_LENGTH = 1024;
		void *parameterData = CORE_ALLOC(MAX_MATERIAL_PARAMETER_BYTE_LENGTH);;
		size_t calculatedSize = 0;

		if (params.HasChild("parameters")) {
			for (auto &parameter : params["parameters"].childrenArray) {
				const std::string type = parameter.meta["type"];
				
				// If the resource starts with a colon, it's a special engine resource.
				const bool isEngineResource = parameter.AsString()[0] == ':';
				
				if (type == "texture") {
					if (isEngineResource) {
						const Texture* texture = GetEngineTexture(parameter.AsString().substr(1));
						textures.push_back(texture);
					}
					else {
						AssetID texID = RequestAsset(parameter.AsFilepath());
						TextureAsset* textureAsset = TextureAsset::Get(texID);

						assets.push_back(texID);
						textures.push_back(&textureAsset->GetTexture());
					}
				}
				else if (type == "renderTarget") {
					const bool useColorMap = parameter.meta["map"] == "color";
					RenderTarget* renderTarget;

					if (isEngineResource) {
						renderTarget = GetEngineRenderTarget(parameter.AsString().substr(1));
					}
					else {
						AssetID rtID = RequestAsset(parameter.AsFilepath());
						renderTarget = RenderTarget::Get(rtID);

						assets.push_back(rtID);
					}

					if (useColorMap) {
						textures.push_back(&renderTarget->GetColorMap());
					}
					else {
						textures.push_back(&renderTarget->GetDepthMap());
					}
				}
				else {
					parameter.AsHLSLType((unsigned char*)parameterData + calculatedSize, type);
				}

				calculatedSize += GetFormatByteSize(GetFormatFromString(type.c_str()));
			}
		}

		// Create the material asset
		bool success = false;
		if (isDynamic) {
			DynamicMaterialAsset *pMat = DynamicMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(pMat != nullptr, false, "Failed to allocate dynamic material.");

			success = pMat->Initialize(shaderFilename, assets, params["parameters"], parameterData, calculatedSize, textures);
		} else {
			StaticMaterialAsset *pMat = StaticMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(pMat != nullptr, false, "Failed to allocate static material.");

			success = pMat->Initialize(shaderFilename, assets, parameterData, calculatedSize, textures);
		}
		
		// Release temp data
		CORE_FREE(parameterData);

		return success;
	}

	static void OnMaterialUnload(const AssetID &id)
	{
		if (StaticMaterialAsset::Exists(id)) {
			return StaticMaterialAsset::Release(id);
		} else if (DynamicMaterialAsset::Exists(id)) {
			return DynamicMaterialAsset::Release(id);
		} else {
			CORE_ASSERT(false, "Unknown material type for material %s", GetAssetFilename(id));
		}
	}




	static bool OnShaderLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header)
	{
		Shader *pShader = Shader::Create(id);
		CORE_ASSERT_RETURN_VALUE(pShader != nullptr, false, "Failed to allocate shader.");

		DeserializationParameterMap params = ParseFile(filename);

		auto vsParams = params["vertexShader"];
		auto psParams = params["pixelShader"];

		// Vertex input
		NGAVertexFormatDesc vertexFormatDesc;
		for (auto &elem : vsParams["inputs"].childrenArray) {
			NGAVertexAttribute attr;

			attr.mSemanticType = GetSemanticType(elem["semanticName"].AsString().c_str());
			attr.mSemanticIndex = elem["semanticIndex"].AsInt(0);

			std::string format = elem["type"].AsString();
			attr.mFormat = GetFormatFromString(format.c_str());

			vertexFormatDesc.mAttributes.push_back(attr);
		}

		return pShader->Initialize(params["file"].AsFilepath(), vertexFormatDesc);
	}

	static void OnShaderUnload(const AssetID &id)
	{
		Shader::Release(id);
	}
}