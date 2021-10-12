#pragma once

#include "GameComponent.h"

#include "Renderer/Light.h"

#include "LightComponent.reflgen.h"

namespace playground
{
	class REFLECTED LightComponent : public GameComponentBase<GameComponentType::LIGHT>
	{
		GENERATED_REFLECTION_CODE();

	public:		
		virtual void Activate() override;
		virtual void Deactivate() override;

		virtual void UpdateLate(float deltaTime) override;

	protected:
		Light mLight REFLECTED;
	};
}