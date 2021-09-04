#pragma once

#include "../SceneEntity.h"

namespace playground
{
	class RenderableInstance : public SceneEntity
	{
	public:
		virtual void Render(bool bindMaterial = true) = 0;
	};
}