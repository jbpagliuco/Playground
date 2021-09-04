#include "GameComponent.h"

#include <map>
#include <string>
#include <vector>

#include "Core/Debug/Assert.h"
#include "Engine/Engine.h"
#include "Engine/World/GameObject.h"

#include "BasicMovementComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "SkyboxComponent.h"
#include "StaticMeshComponent.h"
#include "RigidbodyComponent.h"

#define FOREACH_COMPONENT(func, ...) for (auto &iter : ComponentTable) { for (auto &component : iter.second) { component->func(__VA_ARGS__); } }

#define GENERATE_INSTANTIATOR(type) { #type, &InstantiateGameComponent<type> }

namespace playground
{
	template <typename T>
	GameComponent* InstantiateGameComponent()
	{
		void *mem = CORE_ALLOC(sizeof(T));
		return new (mem) T();
	}

	static std::map<std::string, GameComponent*(*)()> ComponentInstantiators = {
		GENERATE_INSTANTIATOR(BasicMovementComponent),
		GENERATE_INSTANTIATOR(CameraComponent),
		GENERATE_INSTANTIATOR(DirectionalLightComponent),
		GENERATE_INSTANTIATOR(PointLightComponent),
		GENERATE_INSTANTIATOR(RigidbodyComponent),
		GENERATE_INSTANTIATOR(SkyboxComponent),
		GENERATE_INSTANTIATOR(SpotLightComponent),
		GENERATE_INSTANTIATOR(StaticMeshComponent)
	};

	typedef std::vector<GameComponent*> ComponentList;
	static std::map<std::string, ComponentList> ComponentTable;

	void GameComponent::SetOwner(GameObject *owner)
	{
		mOwner = owner;
		mTransform = &(GetOwner()->mTransform);
	}

	GameObject* GameComponent::GetOwner()const
	{
		return mOwner;
	}

	void GameComponent::Deserialize(DeserializationParameterMap &params)
	{
	}

	void GameComponent::Activate()
	{
	}

	void GameComponent::Deactivate()
	{
	}

	void GameComponent::UpdateEarly(float deltaTime)
	{
	}

	void GameComponent::Update(float deltaTime)
	{
	}

	void GameComponent::UpdateLate(float deltaTime)
	{
	}

	void GameComponent::UpdatePhysicsPost()
	{
	}

	void GameComponentDoFrame(float deltaTime)
	{
		FOREACH_COMPONENT(UpdateEarly, deltaTime);
		FOREACH_COMPONENT(Update, deltaTime);
		FOREACH_COMPONENT(UpdateLate, deltaTime);
	}

	void GameComponentShutdown()
	{
		for (auto &iter : ComponentTable) {
			ComponentList components = iter.second;
			for (auto &component : components) {
				CORE_FREE(component);
			}
		}
	}

	void GameComponentPhysicsPost()
	{
		FOREACH_COMPONENT(UpdatePhysicsPost);
	}

	GameComponent* CreateComponentFromType(const std::string &type)
	{
		CORE_ASSERT_RETURN_VALUE(ComponentInstantiators.find(type) != ComponentInstantiators.end(), nullptr, "Failed to find instantiator for component type '%s'. Did you forget to add it to this file?", type.c_str());
		GameComponent *component = ComponentInstantiators[type]();
		CORE_ASSERT_RETURN_VALUE(component != nullptr, nullptr, "Failed to create component of type '%s'", type.c_str());

		if (ComponentTable.find(type) == ComponentTable.end()) {
			ComponentTable[type] = ComponentList();
		}

		ComponentTable[type].push_back(component);

		return component;
	}
}