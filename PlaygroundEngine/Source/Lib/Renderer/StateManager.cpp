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

		success = mPerObjectBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(PerObjectData), MAX_RENDER_OBJECTS);
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize object data buffer.");

		success = mLightsBuffer.Initialize(ConstantBufferUsage::CPU_WRITE, nullptr, sizeof(LightsData));
		CORE_ASSERT_RETURN_VALUE(success, false, "Failed to initialize lights buffer.");

		return true;
	}

	void StateManager::Shutdown()
	{
		mPerFrameBuffer.Shutdown();
		mPerObjectBuffer.Shutdown();
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

		// Release all pending resource updates.
		for (int i = 0; i < mNumPendingResourceUploads; ++i) {
			mPendingResourceUploads[i].Shutdown();
		}
		mNumPendingResourceUploads = 0;
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

	void StateManager::MapPerFrameData(const PerFrameData& perFrameData)
	{
		mPerFrameBuffer.Map(&perFrameData);
	}

	void StateManager::BindPerFrameData()
	{
		GetCurrentCommandContext().BindConstantBuffer(mPerFrameBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::PERFRAME);
	}

	void StateManager::MapPerObjectData(const PerObjectData* perObjectData, size_t numObjects)
	{
		// Map all objects at once.
		mPerObjectBuffer.Map(perObjectData, numObjects * sizeof(PerObjectData));
	}

	void StateManager::BindPerObjectData(int index)
	{
		GetCurrentCommandContext().BindConstantBuffer(mPerObjectBuffer.GetBuffer(), NGA_SHADER_STAGE_VERTEX, (int)ShaderConstantBuffers::OBJECTDATA, index);
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

	void StateManager::UpdateResource(NGABuffer& buffer, void* data, size_t size)
	{
		CORE_ASSERT_RETURN(mNumPendingResourceUploads < MAX_RESOURCE_UPDATES, "Too many pending resource updates.");

		PendingResourceUpdate& pendingData = mPendingResourceUploads[mNumPendingResourceUploads];

		// Copy the data to our blob.
		bool success = pendingData.mUploadBlob.Construct(size, data);
		CORE_ASSERT(success, "Failed to create upload blob.");
		if (!success) {
			pendingData.Shutdown();
			return;
		}

		// Create the upload buffer.
		NGABufferDesc uploadBufferDesc = buffer.GetDesc();
		uploadBufferDesc.mUsage = buffer.GetBufferType() | NGA_BUFFER_USAGE_CPU_WRITE;

		success = pendingData.mUploadBuffer.Construct(uploadBufferDesc);
		CORE_ASSERT(success, "Failed to create upload buffer.");
		if (!success) {
			pendingData.Shutdown();
			return;
		}

		++mNumPendingResourceUploads;
		GetCurrentCommandContext().UpdateResource(buffer, pendingData.mUploadBuffer, pendingData.mUploadBlob.GetBufferPointer());
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