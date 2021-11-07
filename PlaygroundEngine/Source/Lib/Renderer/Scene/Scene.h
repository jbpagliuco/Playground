#pragma once

#include <map>
#include <vector>

#include "Core/DataStructures/Singleton.h"

#include "Camera.h"

namespace playground
{
	struct Light;
	class NGAPipelineState;
	class RenderableInstance;

	typedef std::vector<RenderableInstance*> RenderableBucket;
	typedef std::map<const NGAPipelineState*, RenderableBucket> Renderables;

	class Scene : public Singleton<Scene>
	{
	public:
		// Add a renderable to this scene.
		void AddRenderable(RenderableInstance *r);
		// Remove a renderable from this scene.
		void RemoveRenderable(RenderableInstance *r);

		// Add a light to this scene.
		void AddLight(Light *light);
		// Remove a light from this scene.
		void RemoveLight(Light *light);

		// Add a camera to this scene.
		void AddCamera(Camera *camera);
		// Remove a camera from this scene.
		void RemoveCamera(Camera *camera);

		// Get all renderables in this scene.
		const Renderables& GetRenderables()const;
		// Get all lights in this scene.
		const std::vector<Light*>& GetLights()const;
		// Get all cameras in this scene.
		const std::vector<Camera*>& GetCameras()const;

	private:
		// Returns the bucket that this renderable belongs to.
		RenderableBucket& GetRenderableBucket(const RenderableInstance* renderable);

	private:
		// All renderables in this scene.
		Renderables mRenderables;

		// All lights in this scene.
		std::vector<Light*> mLights;

		// All cameras in this scene.
		std::vector<Camera*> mCameras;
	};
}