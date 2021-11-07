#include "Scene.h"

#include <algorithm>

#include "Renderer/Renderer.h"
#include "Renderer/Scene/Camera.h"
#include "Renderer/Scene/Renderables/RenderableInstance.h"

namespace playground
{
	CORE_CREATE_SINGLETON_INSTANCE(Scene);


	RenderableBucket& Scene::GetRenderableBucket(const RenderableInstance* renderable)
	{
		// Find the PSO used for this renderable
		Material* material = renderable->GetMaterial();
		const NGAPipelineState* pso = Renderer::Get()->FindOrCreatePSO(material);

		return mRenderables[pso];
	}

	void Scene::AddRenderable(RenderableInstance *renderable)
	{
		GetRenderableBucket(renderable).push_back(renderable);
	}

	void Scene::RemoveRenderable(RenderableInstance *renderable)
	{
		RenderableBucket& bucket = GetRenderableBucket(renderable);
		bucket.erase(std::remove(bucket.begin(), bucket.end(), renderable), bucket.end());
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

	const Renderables& Scene::GetRenderables()const
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