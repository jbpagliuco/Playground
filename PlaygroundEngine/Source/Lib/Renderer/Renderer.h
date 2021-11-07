#pragma once

#include "Core/DataStructures/Singleton.h"
#include "Core/OS/OS.h"
#include "Core/Util/CRC.h"

#include "RenderDefs.h"
#include "StateManager.h"
#include "Renderer/NGA/NGAResourceViews.h"
#include "Renderer/NGA/NGASwapChain.h"
#include "Renderer/NGA/NGAPipelineState.h"
#include "Renderer/Resources/RenderTarget.h"
#include "Renderer/Resources/Texture.h"

// Convenience macros
#define Playground_Renderer					Renderer::Get()
#define Playground_RendererStateManager		Renderer::Get()->GetStateManager()
#define Playground_SwapChain				Renderer::Get()->GetSwapChain()
#define Playground_MainRenderTarget			Renderer::Get()->GetRenderTarget()

namespace playground
{
	class Camera;
	class Material;
	class NGAPipelineState;

	struct RendererInitParams
	{
		Window mWindow;
		int mWidth;
		int mHeight;
	};

	class Renderer : public Singleton<Renderer>
	{
	public:
		virtual bool Initialize(const RendererInitParams &params);
		virtual void Shutdown();

		virtual void BeginRender();
		virtual void EndRender();

		const Window& GetWindow() { return mWindow; }

		StateManager* GetStateManager() { return &mStateManager; }
		NGASwapChain* GetSwapChain() { return &mSwapChain; }
		RenderTarget* GetRenderTarget() { return &mMainRenderTarget; }

		// Finds or creates a pipeline state object based off the material.
		const NGAPipelineState* FindOrCreatePSO(const Material* material);
		
	private:
		Window mWindow;
		NGASwapChain mSwapChain;

		RenderTarget mMainRenderTarget;
		
		StateManager mStateManager;

		// List of all loaded PSOs.
		std::map<Checksum32, NGAPipelineState> mPSOs;
	};
}