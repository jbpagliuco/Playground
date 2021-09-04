#pragma once

#include <vector>

#include "Core/Math/Transform.h"

#include "Engine/Components/GameComponent.h"

namespace playground
{
	class GameObject
	{
	public:
		GameObject();

		void SetName(const std::string &name);
		std::string GetName()const;

		void AddComponent(GameComponent *component);
		std::vector<GameComponent*>& GetComponents();

		template <typename ComponentType>
		ComponentType* GetComponentOfType();

	public:
		Transform mTransform;

	private:
		std::string mName;
		std::vector<GameComponent*> mComponents;
	};
}

namespace playground
{
	template <typename ComponentType>
	ComponentType* GameObject::GetComponentOfType()
	{
		// yuck
		ComponentType temp;
		GameComponentType searchType = temp.GetType();

		for (auto& comp : mComponents) {
			if (comp->GetType() == searchType) {
				return (ComponentType*)comp;
			}
		}

		return nullptr;
	}
}