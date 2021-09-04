#pragma once

namespace playground
{
	class Camera;
	class Scene;

	class SceneRenderer
	{
	public:
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;

		virtual void RenderScene(Scene &scene, const Camera &camera) = 0;
	};
}