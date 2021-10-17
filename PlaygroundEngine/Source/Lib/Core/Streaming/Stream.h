#pragma once

#include <stdint.h>
#include <string>

#include "Reflection/ReflMarkup.h"
#include "Util/Serialize.h"

#include "Stream.reflgen.h"

#define CORE_SAFE_RELEASE_ASSET_OBJECT(obj) ReleaseAsset(obj->GetID()); obj = nullptr

namespace playground
{
	struct AssetFileHeader
	{
		int mVersion = 1;
	};

	struct REFLECTED AssetID
	{
		GENERATED_REFLECTION_CODE();

		uint64_t mID;

		operator uint64_t()const { return mID; }
	};
	extern AssetID INVALID_ASSET_ID;

	struct AssetType
	{
		typedef bool(*OnLoadXML)(const AssetID&, const std::string&, const AssetFileHeader&, const DeserializationParameterMap&);
		typedef bool(*OnLoadRaw)(const AssetID&, const std::string&, const AssetFileHeader&);
		typedef void(*OnUnload)(const AssetID&);

		// Asset file extension
		std::string mExt;
		
		// Loads the asset as an XML file.
		OnLoadXML mOnLoadXML = nullptr;
		// Does not process the file before loading.
		OnLoadRaw mOnLoadRaw = nullptr;

		// Unloads the asset.
		OnUnload mOnUnload = nullptr;

		int mMinVersion = 1;
		int mMaxVersion = 1;
	};
	void RegisterAssetType(const AssetType &type);

	struct AssetRecord
	{
		AssetRecord();

		AssetID mID;
		uint16_t mRefCount;

		const AssetType *mType;

		void AddRef();
		void DecRef();

		inline bool operator==(const AssetRecord &rhs)const { return mID == rhs.mID; }
		inline bool operator!=(const AssetRecord &rhs)const { return mID != rhs.mID; }
	};
	extern AssetRecord INVALID_ASSET_RECORD;


	AssetID GetAssetID(const std::string &filename);
	const char* GetAssetFilename(const AssetID &id);

	AssetRecord& GetAssetRecord(const AssetID &id);

	AssetID RequestAsset(const std::string &filename);
	void AddAssetRef(const AssetID &id);

	void ReleaseAsset(const AssetID &id);
	void ReleaseAsset(const std::string &filename);


	bool StreamSystemInit();
	void StreamSystemShutdown();

	void StreamSystemDebugRender();
}