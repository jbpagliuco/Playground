#include "StateManager.h"

#include "Renderer/Light.h"
#include "Renderer/Rect.h"
#include "Renderer/Renderer.h"
#include "Renderer/Resources/IndexBuffer.h"
#include "Renderer/Resources/RenderTarget.h"
#include "Renderer/Resources/VertexBuffer.h"
#include "Renderer/Shader/ShaderProgram.h"

namespace playground
{
	StateManager RendererStateManager;


	struct PerFrameData
	{
		Matrix cameraViewProj;
		Matrix lightViewProj[MAX_SHADOWMAPS];
		int mNumShadowCasters;
	};

	struct PerObjectData
	{
		Matrix world;
		Matrix worldInverseTranspose;
	};
	


	StateManager::StateManager() :
		mBoundRenderTarget(&NGARenderTargetView::INVALID),
		mBoundDepthStencilView(&NGADepthStencilView::INVALID)
	{
	}

	bool StateManager::Initialize()
	{
		bool success = mPerFrameBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(PerFrameData));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize per frame buffer.");

		success = mObjectDataBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(PerObjectData));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize object data buffer.");

		success = mLightsBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(LightsData));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize lights buffer.");

		return true;
	}

	void StateManager::Shutdown()
	{
		mPerFrameBuffer.Shutdown();
		mObjectDataBuffer.Shutdown();
		mLightsBuffer.Shutdown();
	}
	
	void StateManager::SetPerFrameData(const Matrix &cameraViewProj, Matrix lightViewProj[MAX_SHADOWMAPS], int numShadowCasters)
	{
		PerFrameData data;
		data.cameraViewProj = cameraViewProj;

		for (int i = 0; i < STATIC_ARRAY_SIZE(data.lightViewProj); ++i) {
			data.lightViewProj[i] = lightViewProj[i];
		}

		data.mNumShadowCasters = numShadowCasters;

		mPerFrameBuffer.Map(&data);

		mCommandContext.BindConstantBuffer(mPerFrameBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::PERFRAME);
	}

	void StateManager::SetObjectTransform(const Matrix &transform)
	{
		PerObjectData data;
		data.world = transform;
		data.worldInverseTranspose = transform.Inverted().Transposed();

		mObjectDataBuffer.Map(&data);

		mCommandContext.BindConstantBuffer(mObjectDataBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::OBJECTDATA);
	}

	void StateManager::SetLightsData(const LightsData &lights)
	{
		mLightsBuffer.Map((void*)&lights);

		mCommandContext.BindConstantBuffer(mLightsBuffer.GetBuffer(), NGA_SHADER_STAGE_PIXEL, (int)ShaderConstantBuffers::LIGHTS);
	}

	void StateManager::SetViewport(const NGARect &rect)
	{
		mCommandContext.SetViewport(rect);
	}

	void StateManager::SetPrimitiveTopology(NGAPrimitiveTopology primTopology)
	{
		mCommandContext.SetPrimitiveTopology(primTopology);
	}

	void StateManager::BindIndexBuffer(const IndexBuffer &ib)
	{
		mCommandContext.BindIndexBuffer(ib.GetBuffer(), NGAIndexBufferType::IBT_32BIT);
	}

	void StateManager::BindVertexBuffer(const VertexBuffer &vb)
	{
		mCommandContext.BindVertexBuffer(vb.GetBuffer(), vb.GetVertexStride());
	}

	void StateManager::BindInputLayout(const NGAInputLayout &inputLayout)
	{
		mCommandContext.BindInputLayout(inputLayout);
	}

	void StateManager::BindShader(const ShaderProgram &shader)
	{
		mCommandContext.BindShader(shader.GetShader());
	}


	void StateManager::BindUserShaderResource(const Texture &texture, NGAShaderStage stage, int slot)
	{
		BindShaderResource(texture.GetShaderResourceView(), stage, slot + (int)TextureRegisters::USER);
	}

	void StateManager::BindUserShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot)
	{
		BindShaderResource(view, stage, slot + (int)TextureRegisters::USER);
	}

	void StateManager::BindShaderResource(const Texture &texture, NGAShaderStage stage, int slot)
	{
		// Can't bind a shader resource if it's already been bound as an output
		if (texture.GetRenderTargetView() == *mBoundRenderTarget) {
			mCommandContext.BindShaderResource(NGAShaderResourceView::INVALID, stage, slot);
			return;
		}

		if (texture.GetDepthStencilView() == *mBoundDepthStencilView) {
			mCommandContext.BindShaderResource(NGAShaderResourceView::INVALID, stage, slot);
			return;
		}

		mCommandContext.BindShaderResource(texture.GetShaderResourceView(), stage, slot);
	}

	void StateManager::BindShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot)
	{
		mCommandContext.BindShaderResource(view, stage, slot);
	}


	void StateManager::BindUserConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot)
	{
		BindConstantBuffer(constantBuffer, stage, slot + (int)ShaderConstantBuffers::USER);
	}

	void StateManager::BindConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot)
	{
		CORE_ASSERT_RETURN(stage != NGA_SHADER_STAGE_ALL, "Need to implement this.");

		if (stage & NGA_SHADER_STAGE_VERTEX) {
			mCommandContext.BindConstantBuffer(constantBuffer, NGA_SHADER_STAGE_VERTEX, slot);
		}

		if (stage & NGA_SHADER_STAGE_PIXEL) {
			mCommandContext.BindConstantBuffer(constantBuffer, NGA_SHADER_STAGE_PIXEL, slot);
		}
	}


	void StateManager::BindUserSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot)
	{
		BindSamplerState(samplerState, stage, slot + (int)SamplerStateRegisters::USER);
	}

	void StateManager::BindSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot)
	{
		mCommandContext.BindSamplerState(samplerState, stage, slot);
	}



	void StateManager::ClearRenderTarget(const NGARenderTargetView &renderTargetView, const float *clearColor)
	{
		mCommandContext.ClearRenderTarget(renderTargetView, clearColor);
	}

	void StateManager::ClearDepthStencilView(const NGADepthStencilView &depthStencilView)
	{
		mCommandContext.ClearDepthStencilView(depthStencilView);
	}

	void StateManager::BindRenderTarget(const NGARenderTargetView &renderTargetView, const NGADepthStencilView &depthStencilView)
	{
		mBoundRenderTarget = &renderTargetView;
		mBoundDepthStencilView = &depthStencilView;
		mCommandContext.BindRenderTarget(renderTargetView, depthStencilView);
	}

	void StateManager::MapBufferData(const NGABuffer &buffer, const void *data)
	{
		mCommandContext.MapBufferData(buffer, data);
	}

	void StateManager::DrawIndexed(const IndexBuffer &buffer)
	{
		mCommandContext.DrawIndexed((unsigned int)buffer.GetNumIndices());
	}

	void StateManager::BindPipelineState(const NGAPipelineState& state)
	{
		mCommandContext.BindPipelineState(state);
	}
}