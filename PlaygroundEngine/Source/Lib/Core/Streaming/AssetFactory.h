#pragma once

#include <map>

#include "Stream.h"

#define CORE_FACTORY_SETUP(AssetType) std::map<AssetID, AssetType*> AssetType::_Instances = std::map<AssetID, AssetType*>()

namespace playground
{
	template <typename AssetType>
	class AssetFactory
	{
	public:
		static AssetType* Create(AssetID id);

		static void Release(AssetID id);
		static void ReleaseAll();

		static AssetType* Get(AssetID id);
		static bool Exists(AssetID id);

		static size_t NumInstances();
		static bool ReportEmpty();

		AssetID GetID()const;

	private:
		static std::map<AssetID, AssetType*> _Instances;

		AssetID mID;
	};
}

#include "AssetFactory.inl"