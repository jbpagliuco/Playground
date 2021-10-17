#include "MaterialAsset.h"

#include "Core/Streaming/Stream.h"
#include "Core/File/File.h"
#include "Core/Reflection/ReflSerialize.h"
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

	static bool OnMaterialLoad(const AssetID& id, const std::string& filename, const AssetFileHeader& header, const DeserializationParameterMap& parameters);
	static void OnMaterialUnload(const AssetID &id);

	static bool OnShaderLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header, const DeserializationParameterMap& parameters);
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
		return SetTextureParameter(name, RequestAsset(filename));
	}

	bool MaterialAsset::SetTextureParameter(const std::string& name, AssetID textureId)
	{
		return SetTextureParameter(name, TextureAsset::Get(textureId));
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
		return SetRenderTargetParameter(name, RequestAsset(filename), useColorMap);
	}

	bool MaterialAsset::SetRenderTargetParameter(const std::string& name, AssetID renderTargetId, bool useColorMap)
	{
		return SetRenderTargetParameter(name, RenderTarget::Get(renderTargetId), useColorMap);
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
		matType.mOnLoadDeserialize = OnMaterialLoad;
		matType.mOnUnload = OnMaterialUnload;
		RegisterAssetType(matType);

		AssetType shaderType;
		shaderType.mExt = "shaderx";
		shaderType.mOnLoadDeserialize = OnShaderLoad;
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



	static bool OnMaterialLoad(const AssetID& id, const std::string& filename, const AssetFileHeader& header, const DeserializationParameterMap& parameters)
	{
		MaterialAssetDesc materialDesc;
		ReflectionDeserialize(MaterialAssetDesc::StaticClass(), &materialDesc, parameters);

		// We will use this function as a common place to load in assets for the materials.
		// But it is the material's responsibility to release the assets.
		std::vector<AssetID> assets;
		std::vector<const Texture*> textures;

		// Read in parameters
		constexpr size_t MAX_MATERIAL_PARAMETER_BYTE_LENGTH = 1024;
		void* parameterData = CORE_ALLOC(MAX_MATERIAL_PARAMETER_BYTE_LENGTH);;
		size_t calculatedSize = 0;

		const auto addParameterData = [&parameterData, &calculatedSize](MaterialParameterType type, void* data)
		{
			const std::map<MaterialParameterType, size_t> PARAMETER_SIZES = {
				{ MaterialParameterType::FLOAT, sizeof(decltype(MaterialParameter::mFloat)) },
				{ MaterialParameterType::VECTOR, sizeof(decltype(MaterialParameter::mVector)) },
				{ MaterialParameterType::COLOR, sizeof(decltype(MaterialParameter::mColor)) }
			};

			const size_t size = PARAMETER_SIZES.at(type);
			void* fieldStart = static_cast<unsigned char*>(parameterData) + calculatedSize;
			memcpy(fieldStart, data, size);

			calculatedSize += size;
		};

		for (const auto& parameter : materialDesc.mParameters) {
			switch (parameter.mType) {
			case MaterialParameterType::FLOAT:
				addParameterData(parameter.mType, (void*)&parameter.mFloat);
				break;

			case MaterialParameterType::COLOR:
				addParameterData(parameter.mType, (void*)&parameter.mColor);
				break;

			case MaterialParameterType::VECTOR:
				addParameterData(parameter.mType, (void*)&parameter.mVector);
				break;

			case MaterialParameterType::TEXTURE:
			{
				// Add asset id
				assets.push_back(parameter.mAssetId);

				// Add texture
				textures.push_back(&TextureAsset::Get(parameter.mAssetId)->GetTexture());
				break;
			}

			case MaterialParameterType::RENDER_TARGET:
			case MaterialParameterType::RENDER_TARGET_DEPTH:
			{
				// Add asset id
				assets.push_back(parameter.mAssetId);

				// Add render target texture
				const bool useColorMap = parameter.mType == MaterialParameterType::RENDER_TARGET;
				RenderTarget* renderTarget = RenderTarget::Get(parameter.mAssetId);

				if (useColorMap) {
					textures.push_back(&renderTarget->GetColorMap());
				}
				else {
					textures.push_back(&renderTarget->GetDepthMap());
				}
				break;
			}
			};
		}

		// Create the material asset
		bool success = false;
		if (materialDesc.mIsDynamic) {
			DynamicMaterialAsset *pMat = DynamicMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(pMat != nullptr, false, "Failed to allocate dynamic material.");

			success = pMat->Initialize(materialDesc.mShaderFilename, assets, parameters["mParameters"], parameterData, calculatedSize, textures);
		} else {
			StaticMaterialAsset *pMat = StaticMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(pMat != nullptr, false, "Failed to allocate static material.");

			success = pMat->Initialize(materialDesc.mShaderFilename, assets, parameterData, calculatedSize, textures);
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




	static bool OnShaderLoad(const AssetID &id, const std::string &filename, const AssetFileHeader &header, const DeserializationParameterMap& parameters)
	{
		Shader *pShader = Shader::Create(id);
		CORE_ASSERT_RETURN_VALUE(pShader != nullptr, false, "Failed to allocate shader.");

		ShaderDesc shaderDesc;
		ReflectionDeserialize(ShaderDesc::StaticClass(), &shaderDesc, parameters);

		return pShader->Initialize(shaderDesc);
	}

	static void OnShaderUnload(const AssetID &id)
	{
		Shader::Release(id);
	}
}