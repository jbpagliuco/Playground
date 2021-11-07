#pragma once

#include "../SceneRenderer.h"

namespace playground
{
	// A debug renderer that renders an empty scene.
	class EmptySceneRenderer : public SceneRenderer
	{
	public:
		virtual bool Initialize() override;
		virtual void Shutdown() override;

		virtual void BeginRender(Scene& scene) override;
		virtual void EndRender(Scene& scene) override;

		virtual void RenderScene(Scene& scene, const Camera& camera) override;
	};
}