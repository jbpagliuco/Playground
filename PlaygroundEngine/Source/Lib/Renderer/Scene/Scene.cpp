#include "Scene.h"

#include <algorithm>

#include "Camera.h"
#include "Renderables/RenderableInstance.h"

namespace playground
{
	CORE_CREATE_SINGLETON_INSTANCE(Scene);

	void Scene::AddRenderable(RenderableInstance *r)
	{
		mRenderables.push_back(r);
	}

	void Scene::RemoveRenderable(RenderableInstance *r)
	{
		mRenderables.erase(std::remove(mRenderables.begin(), mRenderables.end(), r), mRenderables.end());
	}

	void Scene::AddLight(Light *light)
	{
		mLights.push_back(light);
	}

	void Scene::RemoveLight(Light *light)
	{
		mLights.erase(std::remove(mLights.begin(), mLights.end(), light), mLights.end());
	}

	void Scene::AddCamera(Camera *camera)
	{
		mCameras.push_back(camera);
	}
	
	void Scene::RemoveCamera(Camera *camera)
	{
		mCameras.erase(std::remove(mCameras.begin(), mCameras.end(), camera), mCameras.end());
	}

	const std::vector<RenderableInstance*>& Scene::GetRenderables()const
	{
		return mRenderables;
	}

	const std::vector<Light*>& Scene::GetLights()const
	{
		return mLights;
	}

	const std::vector<Camera*>& Scene::GetCameras()const
	{
		return mCameras;
	}
}