#pragma once

#include "Core/Streaming/AssetFactory.h"
#include "Core/Reflection/ReflMarkup.h"

#include "Renderer/RenderDefs.h"
#include "ShaderProgram.h"

#include "Shader.reflgen.h"

namespace playground
{
	struct REFLECTED ShaderDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		std::string mFilename					REFLECTED REFL_FILEPATH;
		NGAVertexFormatDesc mVertexFormatDesc	REFLECTED;
	};

	class Shader : public AssetFactory<Shader>
	{
	public:
		bool Initialize(const ShaderDesc& shaderDesc);
		void Shutdown();

		void Bind();

		inline const NGAVertexFormatDesc& GetVertexFormat()const { return mVertexFormat; }

		inline const VertexShader& GetVertexShader()const { return mVertexShader; }
		inline const PixelShader& GetPixelShader()const { return mPixelShader; }

	private:
		NGAVertexFormatDesc mVertexFormat;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

#if RENDER_DEBUG_FEATURE(STORE_NAMES)
		std::string mDebugName;
#endif
	};
}