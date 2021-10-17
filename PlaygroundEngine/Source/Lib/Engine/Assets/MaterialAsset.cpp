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




	
	bool MaterialAsset::Initialize(const MaterialAssetDesc& materialDesc)
	{
		mShaderID = materialDesc.mShaderID;

		return true;
	}

	void MaterialAsset::Shutdown()
	{
		ReleaseAssets();
		mMaterialContainer.Shutdown();
	}

	void MaterialAsset::ReleaseAssets()
	{
		for (auto& asset : mAssets) {
			ReleaseAsset(asset);
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

	

	MaterialAsset::ProcessedParameters MaterialAsset::ProcessParameters(const std::vector<MaterialParameter>& parameters)
	{
		ProcessedParameters output;

		// Read in parameters
		constexpr size_t MAX_MATERIAL_PARAMETER_BYTE_LENGTH = 1024;
		output.mConstantBuffer = CORE_ALLOC(MAX_MATERIAL_PARAMETER_BYTE_LENGTH);
		output.mConstantBufferSize = 0;

		// Adds a non-asset parameter to the constant buffer data.
		const auto addParameterData = [&output](const std::string& name, MaterialParameterType type, void* data)
		{
			const std::map<MaterialParameterType, size_t> PARAMETER_SIZES = {
				{ MaterialParameterType::FLOAT,		sizeof(decltype(MaterialParameter::mFloat)) },
				{ MaterialParameterType::VECTOR,	sizeof(decltype(MaterialParameter::mVector)) },
				{ MaterialParameterType::COLOR,		sizeof(decltype(MaterialParameter::mColor)) }
			};

			const size_t size = PARAMETER_SIZES.at(type);
			void* fieldStart = static_cast<unsigned char*>(output.mConstantBuffer) + output.mConstantBufferSize;
			memcpy(fieldStart, data, size);

			// Update dynamic material parameter map
			output.mDynamicMaterialParameterMap.mBufferMap[name].mLength = size;
			output.mDynamicMaterialParameterMap.mBufferMap[name].mOffset = output.mConstantBufferSize;

			output.mConstantBufferSize += size;
		};

		// Process each parameter.
		for (const auto& parameter : parameters) {
			switch (parameter.mType) {
			case MaterialParameterType::FLOAT:
				addParameterData(parameter.mName, parameter.mType, (void*)&parameter.mFloat);
				break;

			case MaterialParameterType::COLOR:
				addParameterData(parameter.mName, parameter.mType, (void*)&parameter.mColor);
				break;

			case MaterialParameterType::VECTOR:
				addParameterData(parameter.mName, parameter.mType, (void*)&parameter.mVector);
				break;

			case MaterialParameterType::TEXTURE:
			{
				// Add asset id
				mAssets.push_back(parameter.mAssetId);

				// Add texture
				output.mTextures.push_back(&TextureAsset::Get(parameter.mAssetId)->GetTexture());

				// Update dynamic material parameter map
				output.mDynamicMaterialParameterMap.mTextureMap[parameter.mName] = output.mCurrentTextureIndex;
				++output.mCurrentTextureIndex;

				break;
			}

			case MaterialParameterType::RENDER_TARGET:
			case MaterialParameterType::RENDER_TARGET_DEPTH:
			{
				// Add asset id
				mAssets.push_back(parameter.mAssetId);

				// Add render target texture
				const bool useColorMap = parameter.mType == MaterialParameterType::RENDER_TARGET;
				RenderTarget* renderTarget = RenderTarget::Get(parameter.mAssetId);

				if (useColorMap) {
					output.mTextures.push_back(&renderTarget->GetColorMap());
				}
				else {
					output.mTextures.push_back(&renderTarget->GetDepthMap());
				}

				// Update dynamic material parameter map
				output.mDynamicMaterialParameterMap.mTextureMap[parameter.mName] = output.mCurrentTextureIndex;
				++output.mCurrentTextureIndex;

				break;
			}
			};
		}

		return output;
	}


	bool StaticMaterialAsset::Initialize(const MaterialAssetDesc& materialDesc)
	{
		if (!MaterialAsset::Initialize(materialDesc)) {
			return false;
		}

		// Process our parameter list.
		const ProcessedParameters parameters = ProcessParameters(materialDesc.mParameters);

		bool success = mMaterial.Initialize(&GetShader(), parameters.mConstantBuffer, parameters.mConstantBufferSize, parameters.mTextures);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to create static material.");

		mMaterialContainer.Initialize(&mMaterial);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to material container.");

		CORE_FREE(parameters.mConstantBuffer);

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



	bool DynamicMaterialAsset::Initialize(const MaterialAssetDesc& materialDesc)
	{
		if (!MaterialAsset::Initialize(materialDesc)) {
			return false;
		}

		// Process our parameter list.
		const ProcessedParameters parameters = ProcessParameters(materialDesc.mParameters);

		// Initialize material.
		bool success = mMaterial.Initialize(&GetShader(), parameters.mConstantBufferSize, parameters.mDynamicMaterialParameterMap, parameters.mConstantBuffer, parameters.mTextures);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to create static material.");

		// Initialize material container.
		mMaterialContainer.Initialize(&mMaterial);
		MATERIAL_ASSERT_CLEANUP(success, false, "Failed to material container.");

		CORE_FREE(parameters.mConstantBuffer);

		return true;
	}

	void DynamicMaterialAsset::Shutdown()
	{
		MaterialAsset::Shutdown();

		for (auto& it : mDynamicAssets) {
			ReleaseAsset(it.second);
		}

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

	bool DynamicMaterialAsset::SetFloatParameter(const std::string& name, float value)
	{
		return mMaterialContainer.SetFloatParameter(name, value);
	}

	bool DynamicMaterialAsset::SetVectorParameter(const std::string& name, const Vector4f& value)
	{
		return mMaterialContainer.SetVectorParameter(name, value);
	}

	bool DynamicMaterialAsset::SetTextureParameter(const std::string& name, const std::string& filename)
	{
		return SetTextureParameter(name, RequestAsset(filename));
	}

	bool DynamicMaterialAsset::SetTextureParameter(const std::string& name, AssetID textureId)
	{
		return SetTextureParameter(name, TextureAsset::Get(textureId));
	}

	bool DynamicMaterialAsset::SetTextureParameter(const std::string& name, TextureAsset* texture)
	{
		AddAssetRef(texture->GetID());

		ReleaseAssetByKey(name);
		mDynamicAssets[name] = texture->GetID();

		return mMaterialContainer.SetTextureParameter(name, &texture->GetTexture());
	}

	bool DynamicMaterialAsset::SetRenderTargetParameter(const std::string& name, const std::string& filename, bool useColorMap)
	{
		return SetRenderTargetParameter(name, RequestAsset(filename), useColorMap);
	}

	bool DynamicMaterialAsset::SetRenderTargetParameter(const std::string& name, AssetID renderTargetId, bool useColorMap)
	{
		return SetRenderTargetParameter(name, RenderTarget::Get(renderTargetId), useColorMap);
	}

	bool DynamicMaterialAsset::SetRenderTargetParameter(const std::string& name, RenderTarget* renderTarget, bool useColorMap)
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

	void DynamicMaterialAsset::ReleaseAssetByKey(const std::string& parameterName)
	{
		if (mDynamicAssets.find(parameterName) != mDynamicAssets.end()) {
			ReleaseAsset(mDynamicAssets[parameterName]);
			mDynamicAssets.erase(parameterName);
		}
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

		// Create the material asset
		MaterialAsset* materialAsset;
		if (materialDesc.mIsDynamic) {
			materialAsset = DynamicMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(materialAsset != nullptr, false, "Failed to allocate dynamic material.");
		}
		else {
			materialAsset = StaticMaterialAsset::Create(id);
			CORE_ASSERT_RETURN_VALUE(materialAsset != nullptr, false, "Failed to allocate static material.");
		}

		// Initialize material.
		const bool success = materialAsset->Initialize(materialDesc);
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