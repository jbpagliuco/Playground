#pragma once

#include "../SceneEntity.h"

namespace playground
{
	class Material;

	class RenderableInstance : public SceneEntity
	{
	public:
		virtual void Render(bool bindMaterial = true) = 0;

		virtual Material* GetMaterial()const = 0;
	};
}