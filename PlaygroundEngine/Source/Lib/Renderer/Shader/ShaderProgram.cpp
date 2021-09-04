#include "ShaderProgram.h"

namespace playground
{
	void ShaderProgram::Shutdown()
	{
		mShader.Destruct();
	}

	const NGAShader& ShaderProgram::GetShader()const
	{
		return mShader;
	}


	bool VertexShader::Initialize(const std::string &filename, NGAVertexFormatDesc vertexFormatDesc)
	{
		mVertexFormatDesc = vertexFormatDesc;
		return mShader.Construct(filename.c_str(), NGAShaderType::VERTEX);
	}

	NGAVertexFormatDesc VertexShader::GetVertexFormatDesc()const
	{
		return mVertexFormatDesc;
	}


	bool PixelShader::Initialize(const std::string &filename)
	{
		return mShader.Construct(filename.c_str(), NGAShaderType::PIXEL);
	}
}