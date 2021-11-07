#pragma once

#include "Renderer/Scene/SceneRenderer.h"

#include "Renderer/NGA/NGAPipelineState.h"
#include "Renderer/Pipelines/ShadowMap.h"
#include "Renderer/Shader/ShaderConstants.h"

namespace playground
{
	struct Light;
	class Matrix;
	class Shader;

	class ForwardRenderer : public SceneRenderer
	{
	public:
		virtual bool Initialize() override;
		virtual void Shutdown() override;

		virtual void BeginRender(Scene& scene) override;
		virtual void EndRender(Scene& scene) override;

		virtual void RenderScene(Scene &scene, const Camera &camera) override;

	private:
		void MapPerFrameData(Scene& scene);
		void CollectShadowCastingLights(Scene &scene);
		void BuildShadowMaps(Scene &scene, const Camera &camera);
		void RenderSceneToBackBuffer(Scene &scene, const Camera &camera);

	private:
		Light* mShadowCastingLights[MAX_SHADOWMAPS];
		int mNumShadowCastingLights = 0;

		ShadowMapBuilder mShadowMapBuilder;
	};
}