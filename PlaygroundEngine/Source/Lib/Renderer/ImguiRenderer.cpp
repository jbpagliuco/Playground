#include "ImguiRenderer.h"

#include "Core/Core.h"

#if CORE_DEBUG_ENABLE(IMGUI)

#include "Core/OS/OSWin32.h"

#include "Renderer.h"

#include "Core/Debug/DebugImgui.h"

#if CORE_RENDER_API(DX11)
#include "NGA/DX11/NGACoreInternalDX11.h"
#elif CORE_RENDER_API(DX12)
#include "NGA/DX12/NGACoreInternalDX12.h"
#endif

namespace playground
{
	static bool StylePushed = false;
	static bool IsFocused = false;

	bool ImguiRendererWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (IsFocused && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
			return true;
		}
		
		return false;
	}

	bool ImguiRendererSystemInit()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

#if CORE_RENDER_API(DX11)
		if (!ImGui_ImplWin32_Init(Playground_Renderer->GetWindow().handle)) {
			return false;
		}

		if (!ImGui_ImplDX11_Init(NgaDx11State.mDevice, NgaDx11State.mContext)) {
			return false;
		}
#elif CORE_RENDER_API(DX12)
		if (!ImGui_ImplWin32_Init(Playground_Renderer->GetWindow().handle)) {
			return false;
	}

		/*if (!ImGui_ImplDX12_Init(NgaDx12State.mDevice)) {
			return false;
		}*/
#else
#error Unknown Render API
#endif

		RegisterWndProcCallback(ImguiRendererWndProc);

		ImguiRendererSetFocus(IsFocused);

		return true;
	}

	void ImguiRendererSystemShutdown()
	{
#if CORE_RENDER_API(DX11)
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
#elif CORE_RENDER_API(DX12)
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
#endif

		ImGui::DestroyContext();
	}

	void ImguiRendererBeginFrame()
	{
#if CORE_RENDER_API(DX11)
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
#elif CORE_RENDER_API(DX12)
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
#endif

		ImGui::NewFrame();
	}

	void ImguiRendererEndFrame()
	{
		Playground_MainRenderTarget->Bind();

		ImGui::Render();

#if CORE_RENDER_API(DX11)
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#elif CORE_RENDER_API(DX12)
		// ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());
#endif
	}

	void ImguiRendererSetFocus(bool focus)
	{
		IsFocused = focus;

		if (StylePushed) {
			ImGui::PopStyleVar(ImGuiStyleVar_Alpha);
		}

		if (focus) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		} else {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		}

		StylePushed = true;
	}

	bool ImguiRendererGetFocus()
	{
		return IsFocused;
	}
}

#else

#include "Core/OS/OSWin32.h"

namespace playground
{
	bool ImguiRendererWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; }
	bool ImguiRendererSystemInit() { return true; }
	void ImguiRendererSystemShutdown() {}
	void ImguiRendererBeginFrame() {}
	void ImguiRendererEndFrame() {}
	void ImguiRendererSetFocus(bool focus) {}
	bool ImguiRendererGetFocus() { return false; }
}

#endif

