#include "../Debug/Assert.h"

#include <vector>

#include "Core/Debug/Log.h"
#include "Core/Memory/Memory.h"

namespace playground
{
	template <typename AssetType>
	AssetType* AssetFactory<AssetType>::Create(AssetID id)
	{
		if (Exists(id)) {
			CORE_ASSERT(false, "Asset instance with id %lu already exists!", id);
			Release(id);
		}

		AssetType *obj = new (CORE_ALLOC(sizeof(AssetType))) AssetType();
		obj->mID = id;

		_Instances[id] = obj;

		return obj;
	}

	template <typename AssetType>
	void AssetFactory<AssetType>::Release(AssetID id)
	{
		CORE_ASSERT_RETURN(Exists(id), "Asset instance with id %lu doesn't exist!", id);
		AssetType *obj = _Instances[id];
		obj->Shutdown();

		_Instances.erase(id);

		CORE_FREE(obj);
	}

	template <typename AssetType>
	void AssetFactory<AssetType>::ReleaseAll()
	{
		std::vector<AssetID> ids;
		for (auto &it : _Instances) {
			ids.push_back(it.first);
		}

		for (auto &id : ids) {
			Release(id);
		}
	}

	template <typename AssetType>
	AssetType* AssetFactory<AssetType>::Get(AssetID id)
	{
		return _Instances[id];
	}

	template <typename AssetType>
	bool AssetFactory<AssetType>::Exists(AssetID id)
	{
		return _Instances.find(id) != _Instances.end();
	}


	template <typename AssetType>
	size_t AssetFactory<AssetType>::NumInstances()
	{
		return _Instances.size();
	}

	template <typename AssetType>
	bool AssetFactory<AssetType>::ReportEmpty()
	{
		if (NumInstances() > 0) {
			LogInfo("AssetFactory", "Found the following assets at shutdown:");
			for (auto& it : _Instances) {
				LogInfo("AssetFactory", "%s", GetAssetFilename(it.first));
			}
		}

		return true;
	}

	template <typename AssetType>
	AssetID AssetFactory<AssetType>::GetID()const
	{
		return mID;
	}
}