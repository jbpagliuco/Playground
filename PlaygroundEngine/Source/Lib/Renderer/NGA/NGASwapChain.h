#pragma once

#include "NGA/NGACommon.h"

#include "Core/OS/OS.h"

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
		void Present()const;

#if CORE_RENDER_API(DX11)
	private:
		struct IDXGISwapChain *mSwapChain;
#endif

		// TODO: Might be a better way to handle this
		friend class NGARenderTargetView;
	};
}