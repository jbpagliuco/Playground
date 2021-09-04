#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Util/Serialize.h"

namespace playground
{
	class GameObject;
	class Transform;

	extern float Frametime;

	enum class GameComponentType
	{
		CAMERA = 0,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
		RIGIDBODY,
		SKYBOX,
		STATIC_MESH,
		BASIC_MOVEMENT
	};

	class GameComponent
	{
	public:
		void SetOwner(GameObject *owner);
		GameObject* GetOwner()const;

		virtual GameComponentType GetType()const = 0;

		virtual void Deserialize(DeserializationParameterMap &params);

		virtual void Activate();
		virtual void Deactivate();
		
		virtual void UpdateEarly(float deltaTime);
		virtual void Update(float deltaTime);
		virtual void UpdateLate(float deltaTime);

		virtual void UpdatePhysicsPost();

	protected:
		GameObject *mOwner;
		Transform *mTransform;
	};

	template <GameComponentType type>
	class GameComponentBase : public GameComponent
	{
	public:
		virtual GameComponentType GetType()const override { return type; }
	};

	// System Callbacks
	void GameComponentDoFrame(float deltaTime);
	void GameComponentShutdown();

	void GameComponentPhysicsPost();

	GameComponent* CreateComponentFromType(const std::string &type);
}