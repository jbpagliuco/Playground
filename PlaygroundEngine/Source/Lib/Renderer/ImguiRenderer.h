#pragma once

namespace playground
{
	bool ImguiRendererSystemInit();
	void ImguiRendererSystemShutdown();

	void ImguiRendererBeginFrame();
	void ImguiRendererEndFrame();

	void ImguiRendererSetFocus(bool focus);
	bool ImguiRendererGetFocus();
}