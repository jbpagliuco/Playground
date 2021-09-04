#pragma once

#include <string>

#include "Renderer/NGA/NGAShader.h"
#include "Renderer/NGA/NGACommon.h"

namespace playground
{
	class ShaderProgram
	{
	public:
		virtual void Shutdown();

		const NGAShader& GetShader()const;
		
	protected:
		NGAShader mShader;
	};

	class VertexShader : public ShaderProgram
	{
	public:
		virtual bool Initialize(const std::string &filename, NGAVertexFormatDesc vertexFormatDesc);

		NGAVertexFormatDesc GetVertexFormatDesc()const;

	private:
		NGAVertexFormatDesc mVertexFormatDesc;
	};

	class PixelShader : public ShaderProgram
	{
	public:
		virtual bool Initialize(const std::string &filename);
	};
}