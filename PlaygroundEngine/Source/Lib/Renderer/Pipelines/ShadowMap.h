#pragma once

#include "Renderer/Resources/RenderTarget.h"

namespace playground
{
	struct Light;
	class Scene;

	class ShadowMapBuilder
	{
	public:
		bool Initialize(int numShadowMaps);
		void Shutdown();

		void BuildAll(const Scene& scene, const Light** lights, int numLights);

		RenderTarget& GetRenderTarget();

	private:
		void BuildSlice(const Scene& scene, const Light& light, int slice);

	private:
		RenderTarget mRenderTarget;
		int mNumShadowMaps;
	};


	bool ShadowMapSystemInitialize();
	void ShadowMapSystemShutdown();
}