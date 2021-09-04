#pragma once

#include <vector>

#include "Core/DataStructures/Singleton.h"

#include "Camera.h"

namespace playground
{
	class RenderableInstance;
	struct Light;

	class Scene : public Singleton<Scene>
	{
	public:
		void AddRenderable(RenderableInstance *r);
		void RemoveRenderable(RenderableInstance *r);

		void AddLight(Light *light);
		void RemoveLight(Light *light);

		void AddCamera(Camera *camera);
		void RemoveCamera(Camera *camera);

		const std::vector<RenderableInstance*>& GetRenderables()const;
		const std::vector<Light*>& GetLights()const;
		const std::vector<Camera*>& GetCameras()const;

	private:
		std::vector<RenderableInstance*> mRenderables;
		std::vector<Light*> mLights;

		std::vector<Camera*> mCameras;
	};
}