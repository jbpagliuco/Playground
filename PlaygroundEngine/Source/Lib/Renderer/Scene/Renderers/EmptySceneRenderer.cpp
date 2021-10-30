#include "EmptySceneRenderer.h"

namespace playground
{
	bool EmptySceneRenderer::Initialize()
	{
		return true;
	}

	void EmptySceneRenderer::Shutdown()
	{
	}

	void EmptySceneRenderer::BeginRender()
	{
	}

	void EmptySceneRenderer::EndRender()
	{
	}

	void EmptySceneRenderer::RenderScene(Scene& scene, const Camera& camera)
	{	
	}
}