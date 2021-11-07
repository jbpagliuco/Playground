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
		mCommandListOpenCount = 0;

		COMMAND_LIST_SCOPE();

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

	void StateManager::OpenCommandList()
	{
		++mCommandListOpenCount;
		if (mCommandListOpenCount == 1) {
			GetCurrentCommandContext().Reset();
		}
	}

	void StateManager::CloseCommandList()
	{
		CORE_ASSERT(mCommandListOpenCount > 0, "Trying to close a command list that isn't open.");
		if (mCommandListOpenCount <= 0) {
			mCommandListOpenCount = 0;
			return;
		}

		--mCommandListOpenCount;
		if (mCommandListOpenCount > 0) {
			return;
		}

		GetCurrentCommandContext().Close();
		GetCurrentCommandContext().Flush();
	}

	void StateManager::ClearAllUserResources()
	{
		for (int i = 0; i < MAX_NUM_USER_SHADER_RESOURCES; ++i) {
			BindUserShaderResource(NGAShaderResourceView::INVALID, NGA_SHADER_STAGE_ALL, i);
		}

		for (int i = 0; i < MAX_NUM_USER_SAMPLER_STATES; ++i) {
			BindUserSamplerState(NGASamplerState::INVALID, NGA_SHADER_STAGE_ALL, i);
		}
	}

	void StateManager::MapPerFrameData(const Matrix& cameraViewProj, Matrix lightViewProj[MAX_SHADOWMAPS], int numShadowCasters)
	{
		PerFrameData data;
		data.cameraViewProj = cameraViewProj;

		for (int i = 0; i < STATIC_ARRAY_SIZE(data.lightViewProj); ++i) {
			data.lightViewProj[i] = lightViewProj[i];
		}

		data.mNumShadowCasters = numShadowCasters;

		mPerFrameBuffer.Map(&data);
	}

	void StateManager::BindPerFrameData()
	{
		GetCurrentCommandContext().BindConstantBuffer(mPerFrameBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::PERFRAME);
	}

	void StateManager::SetObjectTransform(const Matrix& transform)
	{
		PerObjectData data;
		data.world = transform;
		data.worldInverseTranspose = transform.Inverted().Transposed();

		mObjectDataBuffer.Map(&data);

		GetCurrentCommandContext().BindConstantBuffer(mObjectDataBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::OBJECTDATA);
	}

	void StateManager::MapLightsData(const LightsData& lights)
	{
		mLightsBuffer.Map(&lights);
	}

	void StateManager::BindLightsData()
	{
		GetCurrentCommandContext().BindConstantBuffer(mLightsBuffer.GetBuffer(), NGA_SHADER_STAGE_PIXEL, (int)ShaderConstantBuffers::LIGHTS);
	}

	void StateManager::SetViewport(const NGARect& rect)
	{
		GetCurrentCommandContext().SetViewport(rect);
	}

	void StateManager::SetPrimitiveTopology(NGAPrimitiveTopology primTopology)
	{
		GetCurrentCommandContext().SetPrimitiveTopology(primTopology);
	}

	void StateManager::BindIndexBuffer(const IndexBuffer& ib)
	{
		GetCurrentCommandContext().BindIndexBuffer(ib.GetBuffer());
	}

	void StateManager::BindVertexBuffer(const VertexBuffer& vb)
	{
		GetCurrentCommandContext().BindVertexBuffer(vb.GetBuffer());
	}

	void StateManager::BindInputLayout(const NGAInputLayout& inputLayout)
	{
		GetCurrentCommandContext().BindInputLayout(inputLayout);
	}

	void StateManager::BindShader(const ShaderProgram& shader)
	{
		GetCurrentCommandContext().BindShader(shader.GetShader());
	}


	void StateManager::BindUserShaderResource(const Texture& texture, NGAShaderStage stage, int slot)
	{
		BindShaderResource(texture.GetShaderResourceView(), stage, slot + (int)TextureRegisters::USER);
	}

	void StateManager::BindUserShaderResource(const NGAShaderResourceView& view, NGAShaderStage stage, int slot)
	{
		BindShaderResource(view, stage, slot + (int)TextureRegisters::USER);
	}

	void StateManager::BindShaderResource(const Texture& texture, NGAShaderStage stage, int slot)
	{
		// Can't bind a shader resource if it's already been bound as an output
		if (texture.GetRenderTargetView() == *mBoundRenderTarget) {
			GetCurrentCommandContext().BindShaderResource(NGAShaderResourceView::INVALID, stage, slot);
			return;
		}

		if (texture.GetDepthStencilView() == *mBoundDepthStencilView) {
			GetCurrentCommandContext().BindShaderResource(NGAShaderResourceView::INVALID, stage, slot);
			return;
		}

		GetCurrentCommandContext().BindShaderResource(texture.GetShaderResourceView(), stage, slot);
	}

	void StateManager::BindShaderResource(const NGAShaderResourceView& view, NGAShaderStage stage, int slot)
	{
		if (view != NGAShaderResourceView::INVALID) {
			const bool isBoundForOutput =
				(mBoundRenderTarget != nullptr && view.PointsToSameResource(*mBoundRenderTarget)) ||
				(mBoundDepthStencilView != nullptr && view.PointsToSameResource(*mBoundDepthStencilView));

			if (isBoundForOutput) {
				GetCurrentCommandContext().BindShaderResource(NGAShaderResourceView::INVALID, stage, slot);
				return;
			}
		}

		GetCurrentCommandContext().BindShaderResource(view, stage, slot);
	}


	void StateManager::BindUserConstantBuffer(const NGABuffer& constantBuffer, NGAShaderStage stage, int slot)
	{
		BindConstantBuffer(constantBuffer, stage, slot + (int)ShaderConstantBuffers::USER);
	}

	void StateManager::BindConstantBuffer(const NGABuffer& constantBuffer, NGAShaderStage stage, int slot)
	{
		CORE_ASSERT_RETURN(stage != NGA_SHADER_STAGE_ALL, "Need to implement this.");

		if (stage & NGA_SHADER_STAGE_VERTEX) {
			GetCurrentCommandContext().BindConstantBuffer(constantBuffer, NGA_SHADER_STAGE_VERTEX, slot);
		}

		if (stage & NGA_SHADER_STAGE_PIXEL) {
			GetCurrentCommandContext().BindConstantBuffer(constantBuffer, NGA_SHADER_STAGE_PIXEL, slot);
		}
	}


	void StateManager::BindUserSamplerState(const NGASamplerState& samplerState, NGAShaderStage stage, int slot)
	{
		BindSamplerState(samplerState, stage, slot + (int)SamplerStateRegisters::USER);
	}

	void StateManager::BindSamplerState(const NGASamplerState& samplerState, NGAShaderStage stage, int slot)
	{
		GetCurrentCommandContext().BindSamplerState(samplerState, stage, slot);
	}



	void StateManager::BindRenderTarget(const NGARenderTargetView& renderTargetView, const NGADepthStencilView& depthStencilView)
	{
		mBoundRenderTarget = &renderTargetView;
		mBoundDepthStencilView = &depthStencilView;
		GetCurrentCommandContext().BindRenderTarget(renderTargetView, depthStencilView);
	}

	void StateManager::ClearRenderTarget(const NGARenderTargetView& renderTargetView, const float* clearColor)
	{
		GetCurrentCommandContext().ClearRenderTarget(renderTargetView, clearColor);
	}

	void StateManager::PresentRenderTarget(const NGARenderTargetView& renderTargetView)
	{
		GetCurrentCommandContext().PresentRenderTarget(renderTargetView);
	}


	void StateManager::ClearDepthStencilView(const NGADepthStencilView& depthStencilView)
	{
		GetCurrentCommandContext().ClearDepthStencilView(depthStencilView);
	}

	void StateManager::MapBufferData(const NGABuffer& buffer, const void* data, size_t size)
	{
		GetCurrentCommandContext().MapBufferData(buffer, data, size);
	}

	void StateManager::DrawIndexed(const IndexBuffer& buffer)
	{
		GetCurrentCommandContext().DrawIndexed((unsigned int)buffer.GetNumIndices());
	}

	void StateManager::BindPipelineState(const NGAPipelineState& state)
	{
		GetCurrentCommandContext().BindPipelineState(state);
	}


	NGACommandContext& StateManager::GetCurrentCommandContext()
	{
		return mCommandContexts[Playground_SwapChain->GetBufferIndex()];
	}



	CommandListScope::CommandListScope()
	{
		Playground_RendererStateManager->OpenCommandList();
	}

	CommandListScope::~CommandListScope()
	{
		Playground_RendererStateManager->CloseCommandList();
	}
}