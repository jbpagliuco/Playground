#pragma once

#include "Core/Streaming/AssetFactory.h"
#include "Core/Reflection/ReflMarkup.h"

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
		bool Initialize(const std::string& file, NGAVertexFormatDesc vsFormat);
		bool Initialize(const ShaderDesc& shaderDesc);
		void Shutdown();

		void Bind();

		inline const VertexShader& GetVertexShader()const { return mVertexShader; }
		inline const PixelShader& GetPixelShader()const { return mPixelShader; }

	private:
		VertexShader mVertexShader;
		PixelShader mPixelShader;
	};
}