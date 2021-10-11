#include "Stream.h"

#include <algorithm>
#include <map>

#include "Debug/DebugImgui.h"
#include "pugixml-1.9/pugixml.hpp"

#include "Console/Console.h"
#include "Debug/Assert.h"
#include "File/File.h"

namespace playground
{
	AssetID INVALID_ASSET_ID = { (uint64_t)-1 };
	AssetRecord INVALID_ASSET_RECORD;

	static std::map<std::string, AssetType> AssetTypes;

	static std::map<AssetID, AssetRecord> AssetRecords;
	static std::map<std::string, AssetID> AssetIDs;
	static AssetID NextAssetID = { 1 };


	CONSOLE_BOOL(resources, Resources_debug, false);


	static AssetID CreateOrGetAssetID(const std::string &filename)
	{
		AssetID id = GetAssetID(filename);
		if (id == INVALID_ASSET_ID) {
			id = NextAssetID;
			++NextAssetID.mID;

			AssetIDs[filename] = id;
		}

		return id;
	}

	static AssetRecord& CreateOrGetAssetRecord(const AssetID &id)
	{
		if (AssetRecords.find(id) != AssetRecords.end()) {
			return AssetRecords[id];
		}

		AssetRecord record;
		record.mID = id;
		
		AssetRecords[id] = record;

		return AssetRecords[id];
	}



	AssetRecord::AssetRecord() :
		mID(INVALID_ASSET_ID),
		mRefCount(0)
	{}

	void AssetRecord::AddRef()
	{
		++mRefCount;
	}

	void AssetRecord::DecRef()
	{
		--mRefCount;
	}



	void RegisterAssetType(const AssetType &type)
	{
#if CORE_CONFIG(DEBUG)
		CORE_ASSERT(AssetTypes.find(type.mExt) == AssetTypes.end(), "Asset type '.%s' is already registered.", type.mExt.c_str());
#endif

		AssetTypes[type.mExt] = type;
	}



	AssetID GetAssetID(const std::string &filename)
	{
		if (AssetIDs.find(filename) != AssetIDs.end()) {
			return AssetIDs[filename];
		}

		return INVALID_ASSET_ID;
	}

	const char* GetAssetFilename(const AssetID &id)
	{
		const auto predicate = [id](std::pair<std::string, AssetID> it)
		{
			return it.second == id;
		};

		const auto &filename = std::find_if(AssetIDs.begin(), AssetIDs.end(), predicate);
		if (filename != AssetIDs.end()) {
			return filename->first.c_str();
		}

		return "";
	}

	AssetRecord& GetAssetRecord(const AssetID &id)
	{
		if (AssetRecords.find(id) != AssetRecords.end()) {
			return AssetRecords[id];
		}

		return INVALID_ASSET_RECORD;
	}


	static AssetFileHeader LoadFileHeader(const std::string& filename)
	{
		AssetFileHeader header;

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filename.c_str());

		// If the result is false, this file isn't an XML file, so there's no header to parse.
		// Move on, this is likely a file type that won't be handled by us.
		if (result) {
			pugi::xml_node rootXML = doc.child("root");
			if (rootXML) {
				// It's not strictly required for a file to have a header.
				pugi::xml_node headerXML = rootXML.child("header");
				if (headerXML) {
					CORE_ASSERT(headerXML.child("version"), "Failed to find 'version' node in \"%s\" file header.", filename.c_str());
					header.mVersion = headerXML.child("version").text().as_int();
				}
			}
		}

		return header;
	}

	AssetID RequestAsset(const std::string &filename)
	{
		const std::string ext = GetFileExt(filename);

		CORE_ASSERT_RETURN_VALUE(AssetTypes.find(ext) != AssetTypes.end(), INVALID_ASSET_ID, "Requesting asset '%s', but no registered asset type was found!", filename.c_str());
		AssetType &type = AssetTypes[ext];

		// Load header
		AssetFileHeader header = LoadFileHeader(filename);

		// Check version
		CORE_ASSERT(header.mVersion >= type.mMinVersion && header.mVersion <= type.mMaxVersion, "File \"%s\" version (%d) is out-of-date. Must be in range [%d, %d]. Asset may not load properly in game.",
			filename.c_str(), header.mVersion, type.mMinVersion, type.mMaxVersion);
		
		AssetID id = CreateOrGetAssetID(filename);
		AssetRecord &record = CreateOrGetAssetRecord(id);

		record.AddRef();
		if (record.mRefCount == 1) {
			// The asset hasn't been loaded yet.
			record.mType = &type;
			type.mOnLoad(id, filename, header);
		}

		return id;
	}

	void AddAssetRef(const AssetID &id)
	{
		AssetRecord &record = GetAssetRecord(id);
		CORE_ASSERT_RETURN(record != INVALID_ASSET_RECORD, "Failed to find asset record for id %llu", id);

		record.AddRef();
	}

	void ReleaseAsset(const AssetID &id)
	{
		CORE_ASSERT_RETURN(id != INVALID_ASSET_ID);

		AssetRecord &record = GetAssetRecord(id);
		CORE_ASSERT_RETURN(record != INVALID_ASSET_RECORD);
		
		record.DecRef();
		if (record.mRefCount == 0) {
			record.mType->mOnUnload(id);
			AssetRecords.erase(id);

			AssetIDs.erase(GetAssetFilename(id));
		}
	}

	void ReleaseAsset(const std::string &filename)
	{
		ReleaseAsset(GetAssetID(filename));
	}

	

	bool StreamSystemInit()
	{
		return true;
	}

	void StreamSystemShutdown()
	{
		CORE_ASSERT(AssetRecords.size() == 0, "There were left over streaming asset records at shutdown!");
		CORE_ASSERT(AssetIDs.size() == 0, "There were left over streaming asset ids at shutdown!");
	}

	void StreamSystemDebugRender()
	{
		if (!Resources_debug) {
			return;
		}

		if (ImGui::Begin("Resources", &Resources_debug)) {
			std::vector<AssetRecord> sortedRecords;
			for (auto &it : AssetRecords) {
				// If we were given a parameter, use it to cull our list of resources
				/*if (parameters.size() > 0) {
					if (strstr(GetAssetFilename(it.first), parameters[0].AsString().c_str()) == nullptr) {
						continue;
					}
				}*/

				sortedRecords.push_back(it.second);
			}

			std::sort(sortedRecords.begin(), sortedRecords.end(), [](const AssetRecord &a, const AssetRecord &b) {
				return strcmp(GetAssetFilename(a.mID), GetAssetFilename(b.mID)) < 0;
			});

			for (auto &it : sortedRecords) {
				ImGui::Text("%s - Ref Count: %d", GetAssetFilename(it.mID), it.mRefCount);
			}
		}

		ImGui::End();
	}
}