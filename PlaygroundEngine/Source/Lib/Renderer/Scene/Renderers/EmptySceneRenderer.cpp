#include "EmptySceneRenderer.h"

#include "Renderer.h"

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
		Playground_RendererStateManager->OpenCommandList();
	}

	void EmptySceneRenderer::EndRender()
	{
		Playground_RendererStateManager->CloseCommandList();
	}

	void EmptySceneRenderer::RenderScene(Scene& scene, const Camera& camera)
	{	
	}
}