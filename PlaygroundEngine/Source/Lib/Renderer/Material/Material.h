#pragma once

#include "Renderer/Shader/Shader.h"

namespace playground
{
	constexpr int MATERIAL_TYPE_STATIC = 0;
	constexpr int MATERIAL_TYPE_DYNAMIC = 1;
	
	class Material
	{
	public:
		bool Initialize(Shader *shader);
		virtual void Shutdown();

		virtual void Bind();
		
		virtual int GetMaterialType()const = 0;

		inline Shader* GetShader()const { return mShader; }

	private:
		Shader *mShader;
	};
}