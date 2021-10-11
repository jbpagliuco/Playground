#include "World.h"

#include <algorithm>
#include <map>

#include "pugixml-1.9/pugixml.hpp"
#include "ReflectionRegistry.h"

#include "Core/Debug/Assert.h"
#include "Core/Util/Serialize.h"

#include "Engine/World/GameObject.h"
#include "Engine/Components/GameComponent.h"

namespace playground
{
	CORE_CREATE_SINGLETON_INSTANCE(World);

	void World::AddGameObject(GameObject *obj)
	{
		CORE_ASSERT_RETURN(std::find(mObjects.begin(), mObjects.end(), obj) == mObjects.end(), "Game object '%s' is already in world.", obj->GetName().c_str());

		mObjects.push_back(obj);
	}

	void World::Clear()
	{
		for (auto &obj : mObjects) {
			for (auto &component : obj->GetComponents()) {
				component->Deactivate();
			}

			CORE_FREE(obj);
		}

		mObjects.clear();
	}

	void WorldSystemShutdown()
	{
		World::Get()->Clear();
	}



	static Transform ParseTransformFromXML(const pugi::xml_node &transformXML)
	{
		auto positionXML = transformXML.child("position");
		Vector3f position(0.0f, 0.0f, 0.0f);
		if (positionXML) {
			position.x = positionXML.child("x") ? positionXML.child("x").text().as_float() : 0.0f;
			position.y = positionXML.child("y") ? positionXML.child("y").text().as_float() : 0.0f;
			position.z = positionXML.child("z") ? positionXML.child("z").text().as_float() : 0.0f;
		}

		auto rotationXML = transformXML.child("rotation");
		Quaternion rotation = Quaternion::Identity();
		if (rotationXML) {
			float x = rotationXML.child("x") ? rotationXML.child("x").text().as_float() : 0.0f;
			float y = rotationXML.child("y") ? rotationXML.child("y").text().as_float() : 0.0f;
			float z = rotationXML.child("z") ? rotationXML.child("z").text().as_float() : 0.0f;

			if (strcmp(rotationXML.attribute("type").as_string(), "lookat") == 0) {
				// Special case. (X, Y, Z) is the point we are looking at.
				const Vector forwardAxis(0.0f, 0.0f, 1.0f, 0.0f);
				const Vector lookAt = Vector(x - position.x, y - position.y, z - position.z, 0.0f).V3Normalize();

				const float dot = forwardAxis.V3Dot(lookAt);
				if (fabsf(dot + 1.0f) < 0.000001f) {
					rotation = Quaternion(Vector3f(0.0f, 1.0f, 0.0f), PI);
				} else if (fabsf(dot - 1.0f) < 0.000001f) {
					rotation = Quaternion::Identity();
				} else {
					const float rotAngle = acosf(dot);
					const Vector rotAxis = forwardAxis.V3Cross(lookAt).V3Normalize();
					rotation = Quaternion(rotAxis.AsVector3(), rotAngle);
				}
			} else {
				rotation = Quaternion::FromEuler(ToRadians(x), ToRadians(y), ToRadians(z));
			}
		}

		auto scaleXML = transformXML.child("scale");
		Vector3f scale(1.0f, 1.0f, 1.0f);
		if (scaleXML) {
			scale.x = scaleXML.child("x") ? scaleXML.child("x").text().as_float() : 1.0f;
			scale.y = scaleXML.child("y") ? scaleXML.child("y").text().as_float() : 1.0f;
			scale.z = scaleXML.child("z") ? scaleXML.child("z").text().as_float() : 1.0f;
		}

		return Transform(Vector(position), rotation, Vector(scale));
	}

	void LoadWorldFromFile(const std::string &filename)
	{
		World::Get()->Clear();

		std::vector<GameObject*> gameObjects;

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filename.c_str());
		CORE_FATAL_ERROR(result, "Failed to load world file '%s'", filename.c_str());

		pugi::xml_node root = doc.child("root");

		for (auto &gameObjectXML : root) {
			GameObject *obj = new (CORE_ALLOC(sizeof(GameObject))) GameObject();

			const std::string name = gameObjectXML.child("name").text().as_string();
			obj->SetName(name);

			obj->mTransform = gameObjectXML.child("transform") ? ParseTransformFromXML(gameObjectXML.child("transform")) : Transform();

			// Components
			std::vector<GameComponent*> gameObjectComponents;
			pugi::xml_node componentsArrayXML = gameObjectXML.child("components");
			for (auto &componentXML : componentsArrayXML.children("component")) {
				// Create the component
				const std::string componentType = "playground::" + std::string(componentXML.attribute("type").value());
				
				const refl::Class* componentClass = refl::GetSystemRegistry().GetClass(componentType);
				CORE_ASSERT_CONTINUE(componentClass != nullptr, "Unknown component type: %s", componentType.c_str());

				GameComponent *component = CreateComponentFromType(componentType);

				// Deserialize the component
				DeserializationParameterMap params = ParseNode(componentXML);
				ReflectionDeserialize(*componentClass, component, params);
				component->DeserializePost(params);

				// Add the component to this game objects
				obj->AddComponent(component);
				component->SetOwner(obj);
			}

			// Activate all the components on the game object
			for (auto& component : obj->GetComponents()) {
				component->Activate();
			}

			World::Get()->AddGameObject(obj);
		}
	}
}