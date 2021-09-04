#include "GameObject.h"

#include "Core/Util/Util.h"
#include "Engine/Components/GameComponent.h"

namespace playground
{
	GameObject::GameObject()
	{
		mTransform = Transform::Identity();
	}

	void GameObject::SetName(const std::string &name)
	{
		mName = name;
	}

	std::string GameObject::GetName()const
	{
		return mName;
	}

	void GameObject::AddComponent(GameComponent *component)
	{
		mComponents.push_back(component);
	}

	std::vector<GameComponent*>& GameObject::GetComponents()
	{
		return mComponents;
	}
}