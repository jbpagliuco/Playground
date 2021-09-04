#pragma once

#include <string>
#include <vector>

#include "Core/DataStructures/Singleton.h"

namespace playground
{
	class GameObject;

	class World : public Singleton<World>
	{
	public:
		void AddGameObject(GameObject *obj);

		void Clear();
		
	private:
		std::vector<GameObject*> mObjects;
	};

	void WorldSystemShutdown();

	void LoadWorldFromFile(const std::string &filename);
}