#include "GameComponent.h"

#include <map>
#include <string>
#include <vector>

#include "ReflectionRegistry.h"

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

namespace playground
{
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

	void GameComponent::DeserializePost(const DeserializationParameterMap&)
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
		// Find reflected class info.
		const refl::Class* reflClass = refl::GetSystemRegistry().GetClass(type);
		CORE_ASSERT_RETURN_VALUE(reflClass != nullptr, nullptr, "Component class '%s' is not reflected.", type.c_str());

		// Allocate and construct object
		void *obj = CORE_ALLOC(reflClass->mSize);
		GameComponent* component = reflClass->Construct<GameComponent>(obj);

		// Add to component list
		if (ComponentTable.find(type) == ComponentTable.end()) {
			ComponentTable[type] = ComponentList();
		}
		ComponentTable[type].push_back(component);

		return component;
	}
}