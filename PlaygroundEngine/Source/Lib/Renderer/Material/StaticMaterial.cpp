#include "StaticMaterial.h"

#include "Renderer/Renderer.h"
#include "Renderer/Resources/Texture.h"

namespace playground
{
	bool StaticMaterial::Initialize(const StaticMaterialDesc& desc)
	{
		if (!Material::Initialize(desc.mShader, desc.mName)) {
			return false;
		}

		if (desc.mParameterDataByteLength > 0 && !mConstantBuffer.Initialize(ConstantBufferUsage::IMMUTABLE, desc.mParameterData, desc.mParameterDataByteLength)) {
			return false;
		}

		mTextures = desc.mTextures;

		// Make sure all of these textures are shader resources
		for (auto& texture : mTextures) {
			RENDER_ASSERT_RETURN_VALUE(texture->IsShaderResource(), false, "Static material was given a texture that is not a shader resource.");
		}

		return true;
	}
	
	void StaticMaterial::Shutdown()
	{
		Material::Shutdown();

		mConstantBuffer.Shutdown();
	}

	void StaticMaterial::Bind()
	{
		Material::Bind();

		// Bind constant data
		const NGABuffer& constantBuffer = mConstantBuffer.GetBuffer();
		if (constantBuffer.IsConstructed()) {
			Playground_RendererStateManager->BindUserConstantBuffer(constantBuffer, NGA_SHADER_STAGE_PIXEL, 0);
		}

		// Bind textures
		for (int i = 0; i < mTextures.size(); ++i) {
			Playground_RendererStateManager->BindUserShaderResource(*mTextures[i], NGA_SHADER_STAGE_PIXEL, i);
			Playground_RendererStateManager->BindUserSamplerState(mTextures[i]->GetSamplerState(), NGA_SHADER_STAGE_PIXEL, i);
		}
	}
}