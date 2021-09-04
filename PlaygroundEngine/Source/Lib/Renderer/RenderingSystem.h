#pragma once

#include <string>

namespace playground
{
	class Mesh;
	class RenderTarget;
	class Shader;
	class Texture;


	enum class EngineMesh
	{
		QUAD
	};

	enum class EngineShader
	{
		DEBUG_TEXTURE,
		SHADOWMAP,
		SKYBOX,
		BUILD_SSAO_NORMAL_DEPTH
	};


	bool RenderingSystemInit();
	bool RenderingSystemInitLate();
	void RenderingSystemShutdown();

	void RenderingSystemBeginFrame();
	void RenderingSystemDoFrame();
	void RenderingSystemEndFrame();


	void RegisterEngineTexture(const std::string& name, const Texture* texture);
	const Texture* GetEngineTexture(const std::string& name);
	const Texture* TryGetEngineTexture(const std::string& name);

	void RegisterEngineRenderTarget(const std::string& name, RenderTarget* renderTarget);
	RenderTarget* GetEngineRenderTarget(const std::string& name);

	Mesh* GetEngineMesh(EngineMesh mesh);
	Shader* GetEngineShader(EngineShader shader);
}