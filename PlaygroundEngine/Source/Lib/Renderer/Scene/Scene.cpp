#include "Scene.h"

#include <algorithm>

#include "Renderer/Renderer.h"
#include "Renderer/Scene/Camera.h"
#include "Renderer/Scene/Renderables/RenderableInstance.h"

namespace playground
{
	CORE_CREATE_SINGLETON_INSTANCE(Scene);

	static int NextRenderableIndex = 0;


	RenderableBucket& Scene::GetRenderableBucket(const RenderableInstance* renderable)
	{
		// Find the PSO used for this renderable
		Material* material = renderable->GetMaterial();
		const NGAPipelineState* pso = Renderer::Get()->FindOrCreatePSO(material);

		return mRenderables[pso];
	}

	void Scene::AddRenderable(RenderableInstance *renderable)
	{
		// Assign this renderable an index
		SceneRenderable sceneRenderable;
		sceneRenderable.mRenderable = renderable;
		sceneRenderable.mObjectIndex = NextRenderableIndex;
		++NextRenderableIndex;

		GetRenderableBucket(renderable).push_back(sceneRenderable);
	}

	void Scene::RemoveRenderable(RenderableInstance *renderable)
	{
		RenderableBucket& bucket = GetRenderableBucket(renderable);

		const auto predicate = [renderable](const SceneRenderable& it) {
			return it.mRenderable == renderable;
		};
		bucket.erase(std::remove_if(bucket.begin(), bucket.end(), predicate), bucket.end());
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