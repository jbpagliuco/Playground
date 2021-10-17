#pragma once

#include "Core/Math/Matrix.h"

#include "NGA/NGACommandContext.h"

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

		void ClearAllUserResources();

		void SetPerFrameData(const Matrix &cameraViewProj, Matrix lightViewProj[MAX_SHADOWMAPS], int numShadowCasters);
		void SetObjectTransform(const Matrix &transform);

		void SetLightsData(const LightsData &lights);

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

		void ClearRenderTarget(const NGARenderTargetView &renderTargetView, const float *clearColor);
		void ClearDepthStencilView(const NGADepthStencilView &depthStencilView);

		void BindRenderTarget(const NGARenderTargetView &renderTargetView, const NGADepthStencilView &depthStencilView);

		void MapBufferData(const NGABuffer &buffer, const void *data);

		void DrawIndexed(const IndexBuffer &buffer);

		void BindPipelineState(const NGAPipelineState& state);
		
	private:
		NGACommandContext mCommandContext;

		ConstantBuffer mPerFrameBuffer;
		ConstantBuffer mObjectDataBuffer;

		ConstantBuffer mLightsBuffer;

		const NGARenderTargetView *mBoundRenderTarget;
		const NGADepthStencilView *mBoundDepthStencilView;
	};
}