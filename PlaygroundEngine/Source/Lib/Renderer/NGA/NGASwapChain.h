#pragma once

#include "NGA/NGACommon.h"

#include "Core/OS/OS.h"

struct IDXGISwapChain;

namespace playground
{
	struct NGASwapChainDesc
	{
		Window mWindow = INVALID_WINDOW;
		int mBufferCount = 2;
	};

	class NGASwapChain
	{
		NGA_GPU_CLASS(NGASwapChain);

	public:
		bool Construct(const NGASwapChainDesc &desc);
		void Destruct();

		bool IsConstructed()const;

		void SetFullscreenState(bool fullscreen)const;
		void Present();

	private:
#if CORE_RENDER_API(DX11)
		struct IDXGISwapChain* mSwapChain;
#elif CORE_RENDER_API(DX12)
		IDXGISwapChain* mSwapChain;
#endif

		// Index to whichever back buffer we're currently rendering to.
		int mCurrentBackBufferIndex = 0;

		// TODO: Might be a better way to handle this
		friend class NGARenderTargetView;
	};
}