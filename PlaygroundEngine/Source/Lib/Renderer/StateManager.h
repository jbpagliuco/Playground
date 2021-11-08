#pragma once

#include "Core/Math/Matrix.h"

#include "NGA/NGABlob.h"
#include "NGA/NGACommandContext.h"
#include "NGA/NGASwapChain.h"

#include "Renderer/Light.h"
#include "Renderer/Resources/ConstantBuffer.h"
#include "Renderer/Shader/ShaderConstants.h"

#define COMMAND_LIST_SCOPE() CommandListScope __commandListScope;

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

	constexpr int MAX_RENDER_OBJECTS = 64;

	constexpr int MAX_RESOURCE_UPDATES = 64;

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

	// Constant buffers
	struct PerFrameData
	{
		Matrix cameraViewProj;
		Matrix lightViewProj[MAX_SHADOWMAPS];
		int mNumShadowCasters;
	};

	CORE_ALIGN_MS(256) struct PerObjectData
	{
		Matrix mWorld;
		Matrix mWorldInverseTranspose;
	} CORE_ALIGN_GCC(256);

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
		// Closes and flushes the command list.
		void CloseCommandList();

		// Clears all user-bound resources from the graphics pipeline.
		void ClearAllUserResources();

		// Sets per-frame data. Should be called once before rendering the scene.
		void MapPerFrameData(const PerFrameData& perFrameData);
		void BindPerFrameData();
		
		// Sets data for the object about to be rendered.
		void MapPerObjectData(const PerObjectData* perObjectData, size_t numObjects);
		void BindPerObjectData(int index);

		// Sets light data.
		void MapLightsData(const LightsData& lights);
		void BindLightsData();

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

		// Map data to a buffer.
		void MapBufferData(const NGABuffer &buffer, const void *data, size_t size);
		template <typename T>
		void MapBufferData(const NGABuffer& buffer, const T* data)
		{
			MapBufferData(buffer, data, sizeof(T));
		}

		void DrawIndexed(const IndexBuffer &buffer);

		void BindPipelineState(const NGAPipelineState& state);

		// Updates a resource. This should be used upon creation of an immutable resource to set its initial data.
		void UpdateResource(NGABuffer& buffer, void* data, size_t size);
		
	private:
		NGACommandContext& GetCurrentCommandContext();

	private:
		NGACommandContext mCommandContexts[MAX_SWAP_CHAIN_BUFFERS];

		ConstantBuffer mPerFrameBuffer;
		ConstantBuffer mPerObjectBuffer;

		ConstantBuffer mLightsBuffer;

		const NGARenderTargetView *mBoundRenderTarget;
		const NGADepthStencilView *mBoundDepthStencilView;

		int mCommandListOpenCount = 0;
	
		// List of pending resource uploads. Once the command list has been flushed, these can be destroyed.
		struct PendingResourceUpdate
		{
			// The intermediate upload buffer.
			NGABuffer mUploadBuffer;
			// The blob containing the upload data.
			NGABlob mUploadBlob;

			void Shutdown()
			{
				mUploadBlob.Destruct();
				mUploadBuffer.Destruct();
			}
		};
		PendingResourceUpdate mPendingResourceUploads[MAX_RESOURCE_UPDATES];
		int mNumPendingResourceUploads = 0;
	};

	// Opens/Closes the command list 
	class CommandListScope
	{
	public:
		CommandListScope();
		~CommandListScope();
	};
}