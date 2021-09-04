#pragma once

#include <stdint.h>
#include <string>

#define CORE_SAFE_RELEASE_ASSET_OBJECT(obj) ReleaseAsset(obj->GetID()); obj = nullptr

namespace playground
{
	struct AssetFileHeader
	{
		int mVersion = 1;
	};

	typedef uint64_t AssetID;
	extern AssetID INVALID_ASSET_ID;

	struct AssetType
	{
		std::string mExt;
		bool(*mOnLoad)(const AssetID &id, const std::string &filename, const AssetFileHeader &header);
		void(*mOnUnload)(const AssetID &id);

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