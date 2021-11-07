#include "EmptySceneRenderer.h"

#include "Renderer.h"
#include "Renderer/Scene/Scene.h"

namespace playground
{
	bool EmptySceneRenderer::Initialize()
	{
		return true;
	}

	void EmptySceneRenderer::Shutdown()
	{
	}

	void EmptySceneRenderer::BeginRender(Scene& scene)
	{
	}

	void EmptySceneRenderer::EndRender(Scene& scene)
	{
	}

	void EmptySceneRenderer::RenderScene(Scene& scene, const Camera& camera)
	{
		// Bind render target
		Playground_MainRenderTarget->Bind(Playground_SwapChain->GetBufferIndex());
		Playground_MainRenderTarget->Clear(ColorF(COLOR_CORNFLOWERBLUE).FloatArray(), true, Playground_SwapChain->GetBufferIndex());
	}
}