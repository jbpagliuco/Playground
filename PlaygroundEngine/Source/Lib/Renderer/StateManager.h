#pragma once

#include "Core/Math/Matrix.h"

#include "NGA/NGACommandContext.h"
#include "NGA/NGASwapChain.h"

#include "Renderer/Light.h"
#include "Renderer/Resources/ConstantBuffer.h"
#include "Renderer/Shader/ShaderConstants.h"

namespace playground
{
	class NGABuffer;
	class NGAInputLayout;
	class NGASamplerState;
	class NGAShaderResourceView;
	class IndexBuffer;
	class RenderTarget;
	class ShaderProgram;
	class Texture;
	class VertexBuffer;

	struct Rect;

	constexpr int MAX_NUM_USER_SHADER_RESOURCES = 4;
	constexpr int MAX_NUM_USER_SAMPLER_STATES = MAX_NUM_USER_SHADER_RESOURCES;

	enum class ShaderConstantBuffers
	{
		PERFRAME = 0,
		OBJECTDATA,
		LIGHTS,
		USER
	};

	enum class TextureRegisters
	{
		SHADOWMAP = 0,
		USER
	};

	enum class SamplerStateRegisters
	{
		SHADOWMAP = 0,
		USER
	};


	struct LightsData
	{
		Tuple4f globalAmbient;
		Tuple3f eyePosition;
		int numLights;
		Light lights[MAX_LIGHTS];
	};

	class StateManager
	{
	public:
		StateManager();

		bool Initialize();
		void Shutdown();

		// Opens the command list for commands to be added.
		void OpenCommandList();
		// Resets the command list.
		void ResetCommandList();
		// Closes and flushes the command list.
		void CloseCommandList();

		// Clears all user-bound resources from the graphics pipeline.
		void ClearAllUserResources();

		// Sets per-frame data. Should be called once before rendering the scene.
		void SetPerFrameData(const Matrix &cameraViewProj, Matrix lightViewProj[MAX_SHADOWMAPS], int numShadowCasters);
		
		// Sets data for the object about to be rendered.
		void SetObjectTransform(const Matrix &transform);

		// Sets light data.
		void SetLightsData(const LightsData &lights);

		// Set the current viewport.
		void SetViewport(const NGARect &rect);

		void SetPrimitiveTopology(NGAPrimitiveTopology primTopology);

		void BindIndexBuffer(const IndexBuffer &ib);
		void BindVertexBuffer(const VertexBuffer &vb);

		void BindInputLayout(const NGAInputLayout &inputLayout);

		void BindShader(const ShaderProgram &shader);

		void BindUserConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot);
		void BindConstantBuffer(const NGABuffer &constantBuffer, NGAShaderStage stage, int slot);

		void BindUserShaderResource(const Texture &texture, NGAShaderStage stage, int slot);
		void BindUserShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot);
		void BindShaderResource(const Texture &texture, NGAShaderStage stage, int slot);
		void BindShaderResource(const NGAShaderResourceView &view, NGAShaderStage stage, int slot);

		void BindUserSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot);
		void BindSamplerState(const NGASamplerState &samplerState, NGAShaderStage stage, int slot);

		void ClearDepthStencilView(const NGADepthStencilView &depthStencilView);

		void BindRenderTarget(const NGARenderTargetView& renderTargetView, const NGADepthStencilView& depthStencilView);
		void ClearRenderTarget(const NGARenderTargetView& renderTargetView, const float* clearColor);
		void PresentRenderTarget(const NGARenderTargetView& renderTargetView);

		void MapBufferData(const NGABuffer &buffer, const void *data);

		void DrawIndexed(const IndexBuffer &buffer);

		void BindPipelineState(const NGAPipelineState& state);
		
	private:
		NGACommandContext& GetCurrentCommandContext();

	private:
		NGACommandContext mCommandContexts[MAX_SWAP_CHAIN_BUFFERS];

		ConstantBuffer mPerFrameBuffer;
		ConstantBuffer mObjectDataBuffer;

		ConstantBuffer mLightsBuffer;

		const NGARenderTargetView *mBoundRenderTarget;
		const NGADepthStencilView *mBoundDepthStencilView;
	};
}